#pragma once

#include "ForceGenerator.hpp"
#include <vector>

namespace Villain {

    // Keeps track of set of rigid bodies and provides means to simulate them
    // TODO: Collision detection and responses
    class RigidBodyWorld {
        public:
            typedef std::vector<RigidBody*> RigidBodies;

            // Main method for processing physics in rigid body world
            void runPhysics(float deltaTime);

            // Initialises world for a simulation frame which clears all force accumulators for particles
            // in this world, after calling this methods, forces can be added
            void startFrame();

            // Integrate all bodies in this world forward in given delta time
            void integrate(float deltaTime);

            // Getters
            RigidBodies& getBodies() { return bodies; }
            ForceRegistry& getForceRegistry() { return registry; }

        protected:
            RigidBodies bodies; //< All bodies in this world
            ForceRegistry registry; //< All force generators in this world
    };
}
