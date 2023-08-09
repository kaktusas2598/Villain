#include "RigidBodyWorld.hpp"

#include "Engine.hpp"
#include "camera/Camera.hpp"
#include "events/CollisionEvent.hpp"

#include <Logger.hpp>

namespace Villain {

    RigidBodyWorld::RigidBodyWorld(Engine* e, unsigned maxContacts, unsigned iterations)
        : engine(e), resolver(iterations), maxContacts(maxContacts) {
        debugRenderer.init();
        contacts = new Contact[maxContacts];
        calculateIterations = (iterations == 0);
    }

    RigidBodyWorld::~RigidBodyWorld() {
        delete[] contacts;

        for (unsigned int i = 0; i < bodies.size(); i++) {
            delete bodies[i];
            bodies[i] = nullptr;
        }
        bodies.clear();
    }

    void RigidBodyWorld::cast(const glm::vec3& from, const glm::vec3& to, std::function<void(RayHitResult&)> callback, bool closestHitOnly) {
        Ray ray(from, to);

        float closestHitDistance = std::numeric_limits<float>::max();
        RayHitResult closestHitResult;

        // Perform ray casting, find the closest hit and call the callback for each collision found
        for (CollisionPrimitive* collider : colliders) {
            RigidBody* rigidBody = collider->body; // Get the associated rigid body
            RayHitResult result;
            if (collider->intersectRay(ray, result)) {
                float hitDistance = glm::distance(from, result.point);
                if (closestHitOnly) {
                    if (hitDistance < closestHitDistance) {
                        closestHitDistance = hitDistance;
                        closestHitResult = result;
                    }
                } else {
                    // Call the callback for each hit found
                    if (debugDrawEnabled) {
                        debugRenderer.drawLine(from, result.point, {0.9, 0.1, 0.2, 1.0});
                        debugRenderer.drawSphere(result.point, 0.1f, {0.1, 0.1, 0.8, 1.0});
                        debugRenderer.drawLine(result.point, result.point + result.normal , {0.1, 0.1, 0.8, 1.0});
                    }
                    callback(result);
                }
            }
        }

        // If a hit was found and closestHitOnly is true, call the callback with the closest hit result
        if (closestHitOnly && closestHitDistance < std::numeric_limits<float>::max()) {
            if (debugDrawEnabled) {
                debugRenderer.drawLine(from, closestHitResult.point, {0.9, 0.1, 0.2, 1.0});
                debugRenderer.drawSphere(closestHitResult.point, 0.1f, {0.1, 0.1, 0.8, 1.0});
                debugRenderer.drawLine(closestHitResult.point, closestHitResult.point + closestHitResult.normal , {0.1, 0.1, 0.8, 1.0});
            }
            callback(closestHitResult);
        }
    }

    void RigidBodyWorld::startFrame() {
        if (simulationPaused) return;

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

        // TODO: Integrate BVH broad phase here!

        // Narrow phase collision-detection/contact generation
        for (unsigned int i = 0; i < colliders.size(); i++) {
            for (unsigned int j = i + 1; j < colliders.size(); j++) {
                unsigned used = 0;
                // TODO:
                // NOTE: TEMP variables!
                float restitution = 0.4f;
                float friction = 0.1f;
                CollisionData data(nextContact, limit, 0, restitution, friction);

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

                limit -= used;
                nextContact += used;

                // Ran out of contacts to fill!
                if (limit <= 0) break;
            }
        }

        return maxContacts - limit;
    }

    void RigidBodyWorld::runPhysics(float deltaTime) {
        if (simulationPaused) return;

        // First apply all the force generators
        registry.updateForces(deltaTime);

        // Then integrate rigid bodies
        integrate(deltaTime);

        // Generate contacts
        unsigned usedContacts = generateContacts();

        // And process them
        if (calculateIterations) resolver.setIterations(usedContacts * 4);
        if (usedContacts) {
            // Dispatch collision events
            Contact* nextContact = contacts;
            for (int i = 0; i < usedContacts; i++) {
                CollisionEvent collisionEvent = CollisionEvent(nextContact->getBody(0), nextContact->getBody(1));
                engine->getEventDispatcher()->dispatchEvent(collisionEvent);
                nextContact++;
            }
            // Resolve all contacts
            resolver.resolveContacts(contacts, usedContacts, deltaTime);
        }
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
