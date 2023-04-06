#include "PhysicsObject.hpp"

namespace Villain {

    void PhysicsObject::integrate(float deltaTime) {
        position += velocity * deltaTime;
    }
}
