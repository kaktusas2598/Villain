#include "Transform.hpp"

namespace Villain {
    glm::mat4 Transform::getTransformMatrix() {
        if (!needsUpdate) {
            getMatrix();
        }

        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                glm::radians(eulerRot.x),
                glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                glm::radians(eulerRot.y),
                glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                glm::radians(eulerRot.z),
                glm::vec3(0.0f, 0.0f, 1.0f));

        // Y * X * Z
        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        // translation * rotation * scale (also know as TRS matrix)
        model = glm::translate(glm::mat4(1.0f), position) *
            rotationMatrix *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale));

        needsUpdate = false;

        return getMatrix();
    }

    glm::mat4 Transform::getMatrix() const {
        if (parent != nullptr) {
            return parent->getTransformMatrix() * model;
        } else {
            return model;
        }
    }
}
