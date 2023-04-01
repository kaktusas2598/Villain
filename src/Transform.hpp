#ifndef __TRANSFORM__
#define __TRANSFORM__

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Villain {

    // TODO: implement proper transform hierarchy in the scene graph
    class Transform {
        public:
            glm::vec3 Position = glm::vec3(0.0f);
            glm::quat Rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f));
            float Scale = 1.0f;

            glm::mat4 getTransformMatrix() {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
                // TODO: not tested
                model *= glm::mat4_cast(Rotation);
                model = glm::scale(model, glm::vec3(Scale));
                return model;
            }

            void setRot(float angleDeg, glm::vec3 rotationAxis = glm::vec3(0.f, 0.f, 1.f)) {
                Rotation = glm::angleAxis(glm::radians(angleDeg), rotationAxis);
            }
        private:
            Transform* parent;
    };
}

#endif // __TRANSFORM__
