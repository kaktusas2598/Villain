#ifndef __Camera__
#define __Camera__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Frustum.hpp"
#include "Transform.hpp"

namespace Villain {

    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum class ProjectionType {
        NONE,
        ORTHOGRAPHIC,
        ORTHOGRAPHIC_2D,
        PERSPECTIVE
    };

    // Default values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;

    class Camera {
        public:
            Camera(ProjectionType projType = ProjectionType::PERSPECTIVE);
            ~Camera() {}

            // Call after creation and on window size change to change projection
            void rescale(int width, int height);

            glm::mat4 getViewMatrix();
            glm::mat4 getProjMatrix();

            glm::vec3& getPosition() { return position; }
            float getZoom() { return zoom; }
            float getAspectRatio() { return (float)screenWidth / (float)screenHeight; }
            float getZnear() { return zNear; }
            float getZfar() { return zFar; }

            void setPosition(const glm::vec3& newPos) { position = newPos; }
            void setZoom(float z) { zoom = z; }
            void setZPlanes(float near, float far) { zNear = near, zFar = far; }

            // TODO: need more testing, maybe try implementing for other projection types
            Frustum getFrustum();

            // TODO: TEST
            glm::vec3 mouseRayToWorld(const glm::vec2& pos);
            // ORTHOGRAPHIC_2D version
            glm::vec2 screenToWorld(glm::vec2 screenCoords);

            // FROM 2D CAMERA -----------------------
            // Checks if quad is in Camera's clip space
            bool quadInView(const glm::vec2& pos, const glm::vec2& dimensions);

            void offsetPosition(const glm::vec2& offset);
            void offsetScale(float offset);

            // FROM 3D CAMERA -----------------------
            void processKeyboard(CameraMovement direction, float deltaTime);
            void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
            void processMouseScroll(float yOffset);

            glm::vec3 getFront() { return front; }
            glm::vec3 getRight() { return right; }
            glm::vec3 getUp() { return up; }
            glm::vec3 getRotation() { return glm::vec3(pitch, yaw, roll); }
            void setRotation(const glm::vec3& rotation);

        protected:
            // TODO: integrate Transform
            Transform transform;
            int screenWidth, screenHeight;
            float zoom;
            glm::vec3 position;
            ProjectionType projectionType;
            glm::mat4 projection;
            float zNear = 0.1f;
            float zFar = 100.0f;

            // FROM 3D CAMERA --------------------------
            // Using Euler Angles
            void updateCameraVectors();

            // Camera Attributes
            glm::vec3 front;
            glm::vec3 up;
            glm::vec3 right;
            glm::vec3 worldUp;
            // Euler Angles
            float yaw;
            float pitch;
            float roll = 0.0f; // TODO: implement and test
            // Camera Options
            float movementSpeed;
            float mouseSensitivity;
    };
}

#endif // __Camera__
