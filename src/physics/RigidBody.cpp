#include "RigidBody.hpp"

namespace Villain {

    RigidBody::RigidBody()
        : inverseMass(1.0f), // Assuming unit mass by default
        inverseInertiaTensor(glm::mat3(1.0f)),
        inverseInertiaTensorWorld(glm::mat3(1.0f)),
        linearDamping(0.98f), // A reasonable damping value between 0 and 1
        angularDamping(0.8f),
        position(glm::vec3(0.0f)),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        velocity(glm::vec3(0.0f)),
        acceleration(glm::vec3(0.0f)),
        rotation(glm::vec3(0.0f)),
        transformMatrix(glm::mat4(1.0f)),
        isAwake(true)
    {}

    void RigidBody::integrate(float deltaTime) {
        if (!isAwake) return;

        if (inverseMass <= 0.0f) return;  // Infinite mass objects don't move

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

        // Integrate position
        position += velocity * deltaTime;
        // Integrate orientation by converting angular velocity scaled vector to quaternion
        // TODO: Investigate if this is really correct, check page 202 in Ian Millington book
        glm::quat deltaOrientation = 0.5f * ((deltaTime * glm::quat(0.0f, rotation.x, rotation.y, rotation.z)) * orientation);
        orientation = orientation + deltaOrientation;

        // Normalize orientation and update matrices with new position and orientation
        calculateDerivedData();

        // Clear force and torque accumulators
        clearAccumulators();
    }

    void RigidBody::calculateDerivedData() {
        orientation = glm::normalize(orientation);

        calculateTransformMatrix();

        // calculate inertia tensor in world space
        // TODO: Investigate if these calculations are correct
        //inverseInertiaTensorWorld = glm::transpose(glm::inverse(glm::mat3(transformMatrix))) * inverseInertiaTensor * glm::inverse(glm::mat3(transformMatrix));
        // Or this???
        inverseInertiaTensorWorld = glm::mat3(transformMatrix) * inverseInertiaTensor * glm::transpose(glm::mat3(transformMatrix));
        //inverseInertiaTensorWorld = glm::mat3(transformMatrix) * inverseInertiaTensor;
    }

    void RigidBody::calculateTransformMatrix() {
        transformMatrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
    }
}
