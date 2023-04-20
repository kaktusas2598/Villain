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

        // Y * X * Z as in learnopengl scene graph tutorial
        //const glm::mat4 rotationMatrix = transformY * transformX * transformZ;
        // Z * Y * X seems to work with Bullet physics better
        const glm::mat4 rotationMatrix = transformZ * transformY * transformX;

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

    glm::vec3 Transform::getRight() const {
        // 2nd argument to cross product is world up vector
        return glm::normalize(glm::cross(getForward(), glm::vec3(0.0, 1.0f, 0.0f)));
        //return model[0];
    }

    glm::vec3 Transform::getUp() const {
        return glm::normalize(glm::cross(getRight(), getForward()));
        //return model[1];
    }

    glm::vec3 Transform::getBackward() const {
        return -1.f * getForward();
        //return model[2];
    }

    glm::vec3 Transform::getForward() const {
        glm::vec3 front;
        front.x = cos(glm::radians(eulerRot.y)) * cos(glm::radians(eulerRot.x));
        front.y = sin(glm::radians(eulerRot.x));
        front.z = sin(glm::radians(eulerRot.y)) * cos(glm::radians(eulerRot.x));
        return front;
        //return -model[2];
    }

    glm::vec3 Transform::getGlobalScale() const {
        return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
    }
}
