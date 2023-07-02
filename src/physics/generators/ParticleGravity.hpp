#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    class ParticleGravity: public ParticleForceGenerator {
        public:
            ParticleGravity(const glm::vec3& grav) : gravity(grav) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Skip bodies with infinite mass
                if (particle->getInverseMass() <= 0.0f) return;

                // F = m * g
                particle->addForce(gravity * (1.0f / particle->getInverseMass()));
            }

        private:
            glm::vec3 gravity; //< Acceleration due to gravity
    };
}
