#pragma once

#include "physics/ForceGenerator.hpp"

namespace Villain {

    // Connects 2 rigid bodies with spring force
    class Spring : public ForceGenerator {
        public:
            Spring(const glm::vec3& localConnectionPt,
                    RigidBody* otherBody,
                    const glm::vec3& otherConnectionPt,
                    float stiffnessConstant,
                    float springRestLenght) :
                connectionPoint(localConnectionPt),
                otherConnectionPoint(otherConnectionPt),
                other(otherBody),
                springConstant(stiffnessConstant),
                restLenght(springRestLenght)
            {}

            virtual void updateForce(RigidBody* body, float deltaTime) {
                // Calculate two spring ends in world space
                glm::vec3 lws = body->getPointInWorldSpace(connectionPoint);
                glm::vec3 ows = other->getPointInWorldSpace(otherConnectionPoint);

                // Calculate vector of the spring
                glm::vec3 force = lws - ows;

                // Calculate magnitude of the force
                float magnitude = glm::length(force);
                magnitude = fabsf(magnitude - restLenght);
                magnitude *= springConstant;

                // Calculate the final force and apply it
                force = glm::normalize(force);
                force *= -magnitude;
                body->addForceAtPoint(force, lws);
            }

        private:
            glm::vec3 connectionPoint; //< Point of connection of spring in local coordinates
            glm::vec3 otherConnectionPoint; //< Point of connection of spring to the other object in local coordinates
            RigidBody* other; //< Rigid body at the other end of the spring
            float springConstant; //< Stiffness constant of the spring id est Hook's law
            float restLenght; //< Sring's natural rest lenght
    };
}
