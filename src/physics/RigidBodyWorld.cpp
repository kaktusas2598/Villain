#include "RigidBodyWorld.hpp"

#include "camera/Camera.hpp"

#include <Logger.hpp>

namespace Villain {

    RigidBodyWorld::RigidBodyWorld(unsigned maxContacts, unsigned iterations)
        : resolver(iterations), maxContacts(maxContacts) {
        debugRenderer.init();
        contacts = new Contact[maxContacts];
        calculateIterations = (iterations == 0);
    }

    RigidBodyWorld::~RigidBodyWorld() {
        delete[] contacts;
    }

    void RigidBodyWorld::startFrame() {
        for (RigidBody* body : bodies) {
            body->clearAccumulators();
            body->calculateDerivedData();
        }
        for (CollisionPrimitive* collider : colliders) {
            collider->calculateTransform();
        }
    }

    void RigidBodyWorld::integrate(float deltaTime) {
        for (RigidBody* body: bodies) {
            body->integrate(deltaTime);
        }
    }

    unsigned RigidBodyWorld::generateContacts() {
        unsigned limit = maxContacts;
        Contact* nextContact = contacts;

        // TODO: Integrate BVH broad phase here


        // Narrow phase collision-detection/contact generation
        for (unsigned int i = 0; i < colliders.size(); i++) {
            for (unsigned int j = i + 1; j < colliders.size(); j++) {
                unsigned used = 0;
                // NOTE: TEMP!
                CollisionData data(nextContact, limit, 0, 0.1f, 0.0f);

                if (CollisionSphere* one = dynamic_cast<CollisionSphere*>(colliders[i])) {
                    if (CollisionSphere* two = dynamic_cast<CollisionSphere*>(colliders[j])) {
                        used = CollisionDetector::sphereAndSphere(*one, *two, &data);
                    } else if (CollisionBox* two = dynamic_cast<CollisionBox*>(colliders[j])) {
                        used = CollisionDetector::boxAndSphere(*two, *one, &data);
                    } else if (CollisionPlane* two = dynamic_cast<CollisionPlane*>(colliders[j])) {
                        used = CollisionDetector::sphereAndTruePlane(*one, *two, &data);
                    }
                } else if (CollisionBox* one = dynamic_cast<CollisionBox*>(colliders[i])) {
                    if (CollisionSphere* two = dynamic_cast<CollisionSphere*>(colliders[j])) {
                        used = CollisionDetector::boxAndSphere(*one, *two, &data);
                    } else if (CollisionBox* two = dynamic_cast<CollisionBox*>(colliders[j])) {
                        used = CollisionDetector::boxAndBox(*one, *two, &data);
                    } else if (CollisionPlane* two = dynamic_cast<CollisionPlane*>(colliders[j])) {
                        used = CollisionDetector::boxAndHalfSpace(*one, *two, &data);
                    }
                }

                if (used) VILLAIN_TRACE("Used conctacts after iteration {}", used);

                limit -= used;
                nextContact += used;

                // Ran out of contacts to fill!
                if (limit <= 0) break;
            }
        }

        return maxContacts - limit;
    }

    void RigidBodyWorld::runPhysics(float deltaTime) {
        // First apply all the force generators
        registry.updateForces(deltaTime);

        // Then integrate rigid bodies
        integrate(deltaTime);

        // Generate contacts
        unsigned usedContacts = generateContacts();
        if (usedContacts) VILLAIN_TRACE("Generated contacts : {}", usedContacts);

        // And process them
        if (calculateIterations) resolver.setIterations(usedContacts * 4);
        //  NOTE: temp commented out
        resolver.resolveContacts(contacts, usedContacts, deltaTime);
    }

    void RigidBodyWorld::debugDraw(Camera* camera) {
        if (debugDrawEnabled) {
            // Render debug information for collision primitives used in narrow phase
            for (CollisionPrimitive* collider : colliders) {
                collider->debugDraw(&debugRenderer);
            }

            //// TODO: Render debug information for force and contact generators
            ////for (ForceGenerator* forceGenerator : registry.generators()) {
                ////forceGenerator->render(debugRenderer);
            ////}

            debugRenderer.end();
            debugRenderer.render(camera->getProjMatrix() * camera->getViewMatrix(), 1.0f);
        }
    }
}
