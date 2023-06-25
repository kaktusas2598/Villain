#ifndef __TRANSFORM__
#define __TRANSFORM__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Villain {

    class Transform {
        public:
            Transform(const glm::vec3& pos = glm::vec3(0.f), const glm::vec3& rot = glm::vec3(0.f), float sc = 1.0f)
                : position(pos), eulerRot(rot), scale(sc), parent(nullptr), model(1.0f), needsUpdate(true) {}

            glm::mat4 getTransformMatrix();
            bool hasChanged() { return needsUpdate; }

            void translatePosition(const glm::vec3& translation) { position += translation; }
            float getScale() const { return scale; }
            glm::vec3& getPos() { return position; }
            glm::vec3& getEulerRot() { return eulerRot; }
            glm::quat& getOriantation() { return orientation; }
            float* getScalePtr() { return &scale; }

            void setEulerRot(float x, float y, float z) {
                oldEulerRot = eulerRot; eulerRot.x = x; eulerRot.y = y; eulerRot.z = z; needsUpdate = true;
            }
            inline void setPos(const glm::vec3& pos) { position = pos; needsUpdate = true; }
            inline void setRot(const glm::quat& rot) { orientation = rot; }
            inline void setScale(float sc) { scale = sc; needsUpdate = true; }
            inline void setParent(Transform* p) { parent = p; }

            glm::vec3 getRight() const;
            glm::vec3 getUp() const;
            glm::vec3 getBackward() const;
            glm::vec3 getForward() const;
            glm::vec3 getGlobalScale() const;
        private:
            glm::mat4 getMatrix() const;

            glm::vec3 position = glm::vec3(0.0f);
            glm::quat orientation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 0.f));
            glm::vec3 eulerRot = glm::vec3(0.0f);
            float scale = 1.0f;

            glm::vec3 oldPos = glm::vec3(0.0f);
            glm::vec3 oldEulerRot = glm::vec3(0.0f);
            glm::quat oldOrientation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 0.f));
            float oldSc = 1.0f;

            Transform* parent;

            // Store local space matrix and update flag to reduce computational overhead
            glm::mat4 model = glm::mat4(1.0f);
            bool needsUpdate = true;
    };
}

#endif // __TRANSFORM__
