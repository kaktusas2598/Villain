#include "PhysicsEngine.hpp"

namespace Villain {

    void PhysicsEngine::addObject(const PhysicsObject& object) {
        objects.push_back(object);
    }

    void PhysicsEngine::simulate(float deltaTime) {
        for (unsigned int i = 0; i < objects.size(); i++) {
            objects[i].integrate(deltaTime);
        }
    }
}
