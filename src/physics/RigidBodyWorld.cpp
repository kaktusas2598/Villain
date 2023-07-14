#include "RigidBodyWorld.hpp"

#include "camera/Camera.hpp"

namespace Villain {

    RigidBodyWorld::RigidBodyWorld() {
        debugRenderer.init();
    }

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

    void RigidBodyWorld::debugDraw(Camera* camera) {
        if (debugDrawEnabled) {
            // TODO: Implement debug draw methods for rigid body colliders, contacts and force generators
            // Render debug information for rigid bodies
            //for (RigidBody* body : bodies) {
                //body->render(debugRenderer);
            //}

            // Render debug information for force generators
            //for (ForceGenerator* forceGenerator : registry.generators()) {
                //forceGenerator->render(debugRenderer);
            //}

            debugRenderer.end();
            debugRenderer.render(camera->getProjMatrix() * camera->getViewMatrix(), 1.0f);
        }
    }
}
