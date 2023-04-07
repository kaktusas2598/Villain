#include "PhysicsEngine.hpp"

#include "IntersectData.hpp"

namespace Villain {

    void PhysicsEngine::addObject(const PhysicsObject& object) {
        objects.push_back(object);
    }

    void PhysicsEngine::simulate(float deltaTime) {
        for (unsigned int i = 0; i < objects.size(); i++) {
            objects[i].integrate(deltaTime);
        }
    }

    // TODO: will need to implement come kind of spatial partitioning system in physics module and use it here
    // to reduce collision checks
    void PhysicsEngine::handleCollisions() {
        for (unsigned int i = 0; i < objects.size(); i++) {
            for (unsigned int j = i + 1; j < objects.size(); j++) {
                IntersectData intersectData = objects[i].getCollider()
                    .intersect(objects[j].getCollider());
                if (intersectData.isIntersecting()) {
                    // Collision response goes here, ideally detection and response will need to be separate
                    objects[i].setVelocity(objects[i].getVelocity() * -1.f);
                    objects[j].setVelocity(objects[j].getVelocity() * -1.f);
                }
            }
        }

    }
}
