#include "PhysicsObject.hpp"
#include <cstring>

namespace Villain {

    PhysicsObject::PhysicsObject(const PhysicsObject& other) :
        position(other.position),
        oldPosition(other.oldPosition),
        velocity(other.velocity),
        mass(other.mass),
        collider(other.collider)
    {
        // Would add reference for collider in ref counter here
    }

    void PhysicsObject::integrate(float deltaTime) {
        oldPosition = position;
        // TODO: compute forces and torque here
        glm::vec3 linearAcceleration = force/mass;
        // In exclicit numerical integration, position is integrated before velocity, but for systems
        // with changing velocity, semi-implicit euler integration is better (below)
        // NOTE: investigate other numerical integration methods for physics
        if (isDynamic()) {
            // Integrate linear velocity first
            velocity += linearAcceleration * deltaTime; // a = F/m
            position += velocity * deltaTime;

            // TODO: for angular calculations we might need a quaternion in here
            //float angularAccelleration = torque / momentOfIntertia;
            //angularVelocity += angularAccelleration * deltaTime;
        }

        // reset net force
        force = glm::vec3(0.0f);
    }
}
