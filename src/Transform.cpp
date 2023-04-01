#include "Transform.hpp"

namespace Villain {
    glm::mat4 Transform::getTransformMatrix() {
        glm::mat4 model = glm::translate(glm::mat4(1.0), position);
        //model = glm::rotate(model, rotAngle, rotAxis);
        // NOTE: Not working correctly
        // Cause scene to be upside down
        //model *= glm::mat4_cast(rotation);
        model = glm::scale(model, glm::vec3(scale));

        //glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
        //glm::mat4 rotate = glm::mat4_cast(rotation);
        //glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
        //glm::mat4 model = translate * rotate * scaleMat;
        if (parent != nullptr) {
            return parent->getTransformMatrix() * model;
        } else {
            return model;
        }
        //return getParentMatrix() * model;
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
