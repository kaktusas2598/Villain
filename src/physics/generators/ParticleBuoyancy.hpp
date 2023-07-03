#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    // Applied byoyancy force for a plane of liquid parallel to XZ plane
    // This is approximation of buoyancy force using spring-like calculations
    class ParticleBuoyancy: public ParticleForceGenerator {
        public:
            // Density 1000 for pure water
            // 1020 - 1250(Dead sea) for sea water
            ParticleBuoyancy(float maximumDepth, float volume, float height, float density = 1000.0f)
                : maxDepth(maximumDepth), objectVolume(volume), liquidHeight(height), liquidDensity(density) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Get submersion depth
                float depth = particle->getPosition().y;

                // Check if we're out of water
                if (depth >= liquidHeight + maxDepth) return;
                glm::vec3 force = {0.0, 0.0, 0.0};

                // Check if we're at maximum depth
                if (depth <= liquidHeight - maxDepth) {
                    force.y = liquidDensity * objectVolume;
                    particle->addForce(force);
                    return;
                }

                // Otherwise we are partially submerged
                force.y = liquidDensity * objectVolume *
                    (depth - maxDepth - liquidHeight) / 2 * maxDepth;
                particle->addForce(force);
            }

        private:
            float maxDepth; //< Maximum submersion depth of the object before it generates maximum buoyancy force
            float objectVolume; //< Particles actually have no size in our system, so we will assume rectangle shape
            float liquidHeight; //< Height of liquid plane above y=0, plane will be parallel to XZ plane
            float liquidDensity; //< Density of liquid, defaults of pure water density of 1000kg/cm^3
    };
}
