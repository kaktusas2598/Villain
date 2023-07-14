#include "RigidBodyWorld.hpp"

namespace Villain {
    void RigidBodyWorld::startFrame() {
        for (RigidBody* body : bodies) {
            body->clearAccumulators();
            body->calculateDerivedData();
        }
    }

    void RigidBodyWorld::integrate(float deltaTime) {
        for (RigidBody* body: bodies) {
            body->integrate(deltaTime);
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
