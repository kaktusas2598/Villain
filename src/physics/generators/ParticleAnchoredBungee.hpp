#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    // Force generator that applies spring force where onde end is attached to a fixed point in space
    // and spring can only extend like a bungee, but does not compress
    class ParticleAnchoredBungee: public ParticleForceGenerator {
        public:
            ParticleAnchoredBungee(const glm::vec3& anchor, float stifnessConstant, float springRestLength)
                : anchorPoint(anchor), springConstant(stifnessConstant), restLength(springRestLength) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Calculate vector of the spring
                glm::vec3 force = particle->getPosition();
                force -= anchorPoint;

                float magnitude = glm::length(force);
                // Check if the bungee is compressed
                if (magnitude <= restLength) return;

                // Calculate magnitude of the force
                magnitude = springConstant * (restLength - magnitude);

                // Calculate final force and apply it
                force = glm::normalize(force);
                force *= -magnitude;
                particle->addForce(force);
            }

            glm::vec3 getAnchorPoint() const { return anchorPoint; }
            void setAnchorPoint(const glm::vec3& anchor) { anchorPoint = anchor; }

        private:
            glm::vec3 anchorPoint; //< Location of the anchored end of the spring
            float springConstant; //< Spring constant as seen in Hook's law
            float restLength; //< Spring's rest length with no forces acting upon it
    };
}
