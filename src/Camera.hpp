#ifndef __Camera__
#define __Camera__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Villain {

    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    class Camera {
        public:
            // Camera Attributes
            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;
            // Euler Angles
            float Yaw;
            float Pitch;
            // Roll angle could be implemented for fligh simulator camera
            // Camera Options
            float MovementSpeed;
            float MouseSensitivity;
            float Zoom;

            Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

            glm::mat4 getViewMatrix();

            void processKeyboard(CameraMovement direction, float deltaTime);
            void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
            void processMouseScroll(float yOffset);
        private:

            // Using Euler Angles
            void updateCameraVectors();
    };

}

#endif // __Camera__
