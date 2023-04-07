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

    PhysicsObject PhysicsObject::operator=(PhysicsObject other) {
        // This is really not great, wtf, swapping memory, swapping memory
        char* temp[sizeof(PhysicsObject)/sizeof(char)];
        memcpy(temp, this, sizeof(PhysicsObject));
        memcpy(this, &other, sizeof(PhysicsObject));
        memcpy(&other, this,  sizeof(PhysicsObject));
    }

    void PhysicsObject::integrate(float deltaTime) {
        position += velocity * deltaTime;
    }
}
