#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    // Fakes a stiff spring force where one end is attached to a fixed point in space
    // Uses damped harmonic motion and basic drag to predict the force
    // Eg. Could be used to simulate character hair movement
    // High stiffness constant must be used
    class ParticleFakeSpring: public ParticleForceGenerator {
        public:
            ParticleFakeSpring(const glm::vec3& anchor, float stifnessConstant, float damping)
                : anchorPoint(anchor), springConstant(stifnessConstant), dampCoefficient(damping) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                if (!particle->hasFiniteMass()) return;

                // Calculate relative position of the particle to the anchor
                glm::vec3 position = particle->getPosition();
                position -= anchorPoint;

                // Calculate the constants and check if they are in the bounds
                float gamma = 0.5f * sqrtf(4 * springConstant - dampCoefficient * dampCoefficient);
                if (gamma == 0.0f) return;
                glm::vec3 c = position * (dampCoefficient / (2.0f * gamma)) + particle->getVelocity() * (1.0f / gamma);

                // Calculate target position
                glm::vec3 target = position * cosf(gamma * deltaTime) + c * sinf(gamma * deltaTime);
                target *= expf(-0.5f * deltaTime * dampCoefficient);

                // Calculate the resulting acceleration and the force
                glm::vec3 accel = (target - position) * (1.0f / deltaTime * deltaTime) - particle->getVelocity() * deltaTime;
                particle->addForce(accel * particle->getMass());
            }

            glm::vec3 getAnchorPoint() const { return anchorPoint; }
            void setAnchorPoint(const glm::vec3& anchor) { anchorPoint = anchor; }

        private:
            glm::vec3 anchorPoint; //< Location of the anchored end of the spring
            float springConstant; //< Spring constant as seen in Hook's law
            float dampCoefficient; //< Damping value for oscillation of the spring
    };
}
