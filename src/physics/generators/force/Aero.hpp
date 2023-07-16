#pragma once

#include "physics/ForceGenerator.hpp"

namespace Villain {

    // For realistic aerodynamic force behaviour, fluid dynamics of air movement must be considered which
    // is complicated, so for simplifying this, "aerodynamic" (not to be confused with inertia) tensor can be used
    // TODO: Properly test this
    class Aero: public ForceGenerator {
        public:
            Aero(const glm::mat3& aeroTensor, const glm::vec3& pos, glm::vec3* windspeed) :
                tensor(aeroTensor), position(pos), windSpeed(windspeed) {}

            virtual void updateForce(RigidBody* body, float deltaTime) {
                updateForceFromTensor(body, deltaTime, tensor);
            }

        protected:
            // Exactly the same as updateForce, except that this takes explicit tensor matrix
            void updateForceFromTensor(RigidBody* body, float deltaTime, const glm::mat3& aeroTensor) {
                // Calculate total velocity (wind speed and body's velocity)
                glm::vec3 velocity = body->getLinearVelocity();
                velocity += *windSpeed;

                // Calculate the velocity in body coordinates
                glm::vec3 bodyVel = glm::inverse(body->getTransform()) * glm::vec4(velocity, 1.0);

                // Calculate the force in body coordinates
                glm::vec3 bodyForce = aeroTensor * bodyVel;
                glm::vec3 force = body->getTransform() * glm::vec4(bodyForce, 1.0);

                // Finally apply the force
                body->addForceAtBodyPoint(force, position);
            }

            glm::mat3 tensor; //< Aerodynamic tensor for surface in body/local space
            glm::vec3 position; //< Relative position of the aerodynamic surface in body coords
            glm::vec3* windSpeed; //< Pointer to the wind speed of the environment, that way it can be controlled globally
    };
}
