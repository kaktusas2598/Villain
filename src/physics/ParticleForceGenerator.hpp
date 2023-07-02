#pragma once

#include "Particle.hpp"
#include <vector>

namespace Villain {

    // Generator can add a force to one or more particles
    class ParticleForceGenerator {
        public:
            virtual ~ParticleForceGenerator() {}
            // Update force for a given particle
            virtual void updateForce(Particle* particle, float deltaTime) = 0;
    };

    // Holds relationships between all the force generators and the particles they are applied to
    class ParticleForceRegistry {
        public:
            // Register given force generator to apply to given particle
            void add(Particle* particle, ParticleForceGenerator* generator);
            // Unregister given Particle/ForceGenerator pair
            void remove(Particle* particle, ParticleForceGenerator* generator);
            // Clears all registrations, not responsible for deleting particles and generators
            void clear();

            // Main update method to call all generators with their corresponding particles
            void updateForces(float deltaTime);


        protected:
            // 1 to 1 force generator to particle mapping
            struct ParticleForceRegistration {
                Particle* particle;
                ParticleForceGenerator* generator;
            };

            typedef std::vector<ParticleForceRegistration> ParticleRegistry;
            ParticleRegistry registry;
    };
}
