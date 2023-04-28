#include "PhysicsEngine.hpp"

#include "Engine.hpp"
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
                    // TODO: will need to implement proper conservation of momentum here, impulses
                    // so colliders will need to have mass
                    //
                    // Collision response goes here, ideally detection and response will need to be separate
                    //glm::vec3 direction = glm::normalize(intersectData.getDirection());
                    //if (objects[i].getCollider().getType() == ColliderType::AABB || objects[j].getCollider().getType() == ColliderType::AABB) {
                        //printf("Collision direction: %f.1, %f.1, %f.1\n", direction.x, direction.y, direction.z);
                    //}
                    //glm::vec3 otherDirection = glm::reflect(glm::normalize(objects[i].getVelocity()), intersectData.getDirection());
                    //objects[i].setVelocity(glm::reflect(otherDirection, objects[i].getVelocity()));
                    //objects[j].setVelocity(glm::reflect(direction, objects[j].getVelocity()));

                    // Working most basic collision response, just invert velocity
                    if (objects[i].isDynamic())
                        objects[i].setVelocity(objects[i].getVelocity() * -1.f);
                    if (objects[j].isDynamic())
                        objects[j].setVelocity(objects[j].getVelocity() * -1.f);
                }
            }
        }

    }

    void PhysicsEngine::render() {
        if (debugMode) {
            Camera* cam = engine->getRenderingEngine()->getMainCamera();

            for (unsigned int i = 0; i < objects.size(); i++) {
                const_cast<Collider&>(objects[i].getCollider()).render(debugRenderer);
            }

            debugRenderer->end();
            debugRenderer->render(cam->getProjMatrix() * cam->getViewMatrix(), 2.0f);
        }
    }
}
