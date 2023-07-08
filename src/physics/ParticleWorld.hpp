#pragma once

#include "ParticleContact.hpp"
#include "ParticleForceGenerator.hpp"
#include <vector>

namespace Villain {

    // Keeps track of set of particles and provides means to simulate them, generate contacts and handle collisions
    class ParticleWorld {
        public:
            typedef std::vector<Particle*> Particles;
            typedef std::vector<ParticleContactGenerator*> ContactGenerators;

            // If not iterations are provided, when it will be twice the number of contacts used
            ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
            ~ParticleWorld();

            // Main method for processing physics in particle world
            void runPhysics(float deltaTime);

            // Initialises world for a simulation frame which clears all force accumulators for particles
            // in this world, after calling this methods, forces can be added
            void startFrame();

            // Integrate all particles in this world forward in given delta time
            void integrate(float deltaTime);

            // Calls every registered contact generator and returns total number of generated contacts
            unsigned generateContacts();

        protected:
            Particles particles; //< All particles in this world
            ParticleForceRegistry registry; //< All force generators in this world
            ParticleContactResolver resolver; //< Resolver for all contacts in this world
            ContactGenerators contactGenerators; //< All contact generators in this world
            ParticleContact* contacts; //< All contacts
            unsigned numMaxContacts; //< Maximum contacts allowed in this world
            bool calculateIterations;
    };
}
