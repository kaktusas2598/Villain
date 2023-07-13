#pragma once

#include "physics/ForceGenerator.hpp"

namespace Villain {

    // Applies gravitational force to rigid bodies, one instance may be shared with multiple bodies
    class Gravity: public ForceGenerator {
        public:
            Gravity(const glm::vec3& grav) : gravity(grav) {}

            virtual void updateForce(RigidBody* body, float deltaTime) {
                // Skip bodies with infinite mass
                if (!body->hasFiniteMass()) return;

                // F = m * g
                body->addForce(gravity * (1.0f / body->getInverseMass()));
            }

        private:
            glm::vec3 gravity; //< Acceleration due to gravity
    };
}
