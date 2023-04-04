#include "Transform.hpp"

namespace Villain {
    glm::mat4 Transform::getTransformMatrix() {
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
        const glm::mat4 roationMatrix = transformY * transformX * transformZ;

        // translation * rotation * scale (also know as TRS matrix)
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                    roationMatrix *
                    glm::scale(glm::mat4(1.0f), glm::vec3(scale));

        if (parent != nullptr) {
            return parent->getTransformMatrix() * model;
        } else {
            return model;
        }
    }

    glm::mat4 Transform::getParentMatrix() const {
        if (parent != nullptr && parent->hasChanged()) {
            parentMatrix = parent->getTransformMatrix();
        }
        return parentMatrix;
    }

    bool Transform::hasChanged() {

        if (parent != nullptr && parent->hasChanged()) {
            return true;
        }

        if (position != oldPos) {
            oldPos = position;
            return true;
        }

        if (rotation != oldRot) {
            oldRot = rotation;
            return true;
        }

        if (scale != oldSc) {
            oldSc = scale;
            return true;
        }

        return false;
    }

}
