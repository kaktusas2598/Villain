#include "PhysicsObject.hpp"
#include <cstring>

namespace Villain {

    PhysicsObject::PhysicsObject(const PhysicsObject& other) :
        position(other.position),
        oldPosition(other.oldPosition),
        linearVelocity(other.linearVelocity),
        mass(other.mass),
        collider(other.collider)
    {
        // Would add reference for collider in ref counter here
    }

    void PhysicsObject::integrate(float deltaTime) {
        // TEMP force test
        //force += glm::vec3(0.0, -1.0, 0.0);
        oldPosition = position;
        // TODO: compute forces and torque here
        glm::vec3 linearAcceleration = force * inverseMass;
        // In exclicit numerical integration, position is integrated before velocity, but for systems
        // with changing velocity, semi-implicit euler integration is better (below)
        // NOTE: investigate other numerical integration methods for physics
        if (isDynamic()) {
            //momentum += force * deltaTime;
            //linearVelocity = momentum / mass;
            // Integrate linear velocity first
            linearVelocity += linearAcceleration * deltaTime; // a = F/m
            position += linearVelocity * deltaTime;

            // TODO: for angular calculations we might need a quaternion in here
            //float angularAccelleration = torque / momentOfIntertia;
            //
            // updateInertiaTensor()
            // angularAcc = inertiaTensor * torque;
            // angularVelocity += angularAccelleration * deltaTime;
            // update orientation quaternion
            // update angularVelocity again?
        }

        // reset net force
        clearForces();
    }
}
