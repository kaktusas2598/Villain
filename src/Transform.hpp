#ifndef __TRANSFORM__
#define __TRANSFORM__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Villain {

    class Transform {
        public:
            Transform(const glm::vec3& pos = glm::vec3(0.f), const glm::quat& rot = glm::quat(0, 0, 0, 0), float sc = 1.0f)
                : position(pos), rotation(rot), scale(sc), parent(nullptr) {}

            glm::mat4 getTransformMatrix();
            bool hasChanged();

            //const glm::vec3& getPos() const { return position; }
            float getScale() const { return scale; }

            glm::vec3& getPos() { return position; }
            float* getScalePtr() { return &scale; }

            void setRot(float angleDeg, glm::vec3 rotationAxis = glm::vec3(0.f, 0.f, 1.f)) {
                rotation = glm::angleAxis(glm::radians(angleDeg), rotationAxis);
            }
            void setEulerRot(float x, float y, float z) { eulerRot.x = x; eulerRot.y = y; eulerRot.z = z; }
            inline void setPos(const glm::vec3& pos) { position = pos; }
            inline void setScale(float sc) { scale = sc; }
            inline void setParent(Transform* p) { parent = p; }
        private:
            glm::mat4 getParentMatrix() const;

            glm::vec3 position = glm::vec3(0.0f);
            glm::quat rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 0.f));
            // NOTE: for now we will use euler rotations instead of quaternions, but in the future quaternions should be preffered way
            glm::vec3 eulerRot = glm::vec3(0.0f);
            float scale = 1.0f;

            glm::vec3 oldPos = glm::vec3(0.0f);
            glm::quat oldRot = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 0.f));
            float oldSc = 1.0f;

            Transform* parent;
            // From C++11 mutable keybord allows const function to modify these
            mutable glm::mat4 parentMatrix = glm::mat4(1.0f);
    };
}

#endif // __TRANSFORM__
