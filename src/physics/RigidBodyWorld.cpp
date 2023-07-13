#include "RigidBodyWorld.hpp"

namespace Villain {
    void RigidBodyWorld::startFrame() {
        for (RigidBodies::iterator b = bodies.begin(); b != bodies.end(); b++) {
            (*b)->clearAccumulators();
            (*b)->calculateDerivedData();
        }
    }

    void RigidBodyWorld::integrate(float deltaTime) {
        for (RigidBodies::iterator b = bodies.begin(); b != bodies.end(); b++) {
            (*b)->integrate(deltaTime);
        }
    }

    void RigidBodyWorld::runPhysics(float deltaTime) {
        // First apply all the force generators
        registry.updateForces(deltaTime);

        // Then integrate rigid bodies
        integrate(deltaTime);
        // TODO: contacts, constraints, collision detection, response, etc.
    }
}
