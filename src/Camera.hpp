#ifndef __Camera__
#define __Camera__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Frustum.hpp"

namespace Villain {

    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Default values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    //const float SPEED = 20.5f;
    const float SENSITIVITY = 0.1f;

    // Common interface for all Camera types
    class Camera {
        public:
            Camera() {}
            virtual ~Camera() {}

            // Call after creation and on window size change to change projection
            virtual void rescale(int width, int height) = 0;

            virtual glm::mat4 getViewMatrix() = 0;
            virtual glm::mat4 getProjMatrix() = 0;

            glm::vec3& getPosition() { return position; }
            float getZoom() { return zoom; }
            float getAspectRatio() { return (float)screenWidth / (float)screenHeight; }

            void setPosition(const glm::vec3& newPos) { position = newPos; }
            void setZoom(float z) { zoom = z; }

            // NOTE: Obviously 2D camera does not need frustum, but defined it here anyway for now
            // so we don't have to dynamic_cast every time we want to render a mesh and check frustum
            virtual Frustum getFrustum() { return Frustum(); }

        protected:
            int screenWidth, screenHeight;
            float zoom;
            glm::vec3 position;
            glm::mat4 projection;
    };
}

#endif // __Camera__
