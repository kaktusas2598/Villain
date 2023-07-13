#include "RigidBody.hpp"

namespace Villain {

    RigidBody::RigidBody()
        : inverseMass(1.0f), // Assuming unit mass by default
        linearDamping(0.98f), // A reasonable damping value between 0 and 1
        angularDamping(0.98f),
        position(glm::vec3(0.0f)),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f)),
        rotation(glm::vec3(0.0f)),
        transformMatrix(glm::mat4(1.0f))
    {}

    void RigidBody::integrate(float deltaTime) {
        // Calculate linear acceleration from force inputs
        lastFrameAcceleration = acceleration;
        lastFrameAcceleration += forceAccum * inverseMass;

        // Calculate angular acceleration from torque inputs
        glm::vec3 angularAcceleration = inverseInertiaTensorWorld * torqueAccum;

        // Adjust velocities from both acceleration and impulse
        velocity += lastFrameAcceleration * deltaTime;
        rotation += angularAcceleration * deltaTime;

        // Impose drag
        velocity *= powf(linearDamping, deltaTime);
        rotation *= powf(angularDamping, deltaTime);

        // Adjust positions
        position += velocity * deltaTime;
        //orientation += rotation * deltaTime;
        // TODO: ChatGPT line, test this well!
        orientation = glm::normalize(orientation * glm::quat(0.0f, rotation.x, rotation.y, rotation.z) * deltaTime);

        // Normalize orientation and update matrices with new position and orientation
        calculateDerivedData();

        // Clear force and torque accumulators
        clearAccumulators();
    }

    void RigidBody::calculateDerivedData() {
        orientation = glm::normalize(orientation);

        calculateTransformMatrix();

        // calculate inertia tensor in world space
        // TODO: ChatGPT line, test this well!
        inverseInertiaTensorWorld = glm::mat3(transformMatrix) * inverseInertiaTensor;
    }

    void RigidBody::calculateTransformMatrix() {
        transformMatrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
    }
}
