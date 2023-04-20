#include "PhysicsObject.hpp"
#include <cstring>

namespace Villain {

    PhysicsObject::PhysicsObject(const PhysicsObject& other) :
        position(other.position),
        oldPosition(other.oldPosition),
        velocity(other.velocity),
        collider(other.collider)
    {
        // Would add reference for collider in ref counter here
    }

    void PhysicsObject::integrate(float deltaTime) {
        position += velocity * deltaTime;
    }
}
