#include "RigidBody.hpp"

namespace Villain {

    RigidBody::RigidBody()
        : inverseMass(1.0f), // Assuming unit mass by default
        linearDamping(0.98f), // A reasonable damping value between 0 and 1
        position(glm::vec3(0.0f)),
        orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        velocity(glm::vec3(0.0f)),
        rotation(glm::vec3(0.0f)),
        transformMatrix(glm::mat4(1.0f))
    {}

    void RigidBody::integrate(float deltaTime) {
        // TODO: implement
        // Euler integration, D'Alambert, torques, all that jazz

        clearAccumulators();
    }

    void RigidBody::calculateDerivedData() {
        orientation = glm::normalize(orientation);

        calculateTransformMatrix();

        // calculate inertia tensor in world space
        // TODO: ChatGPT code! Test this well!
        glm::mat3 inverseInertiaTensorWorld = glm::mat3(transformMatrix) * inverseInertiaTensor;
    }

    void RigidBody::calculateTransformMatrix() {
        transformMatrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
    }

}
