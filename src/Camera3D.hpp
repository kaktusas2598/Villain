#ifndef __Camera3D__
#define __Camera3D__

#include "Camera.hpp"

namespace Villain {

    // Perspective projection camera
    class Camera3D : public Camera {
        public:
            Camera3D(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

            // Call after creation and on window size change
            void rescale(int width, int height);

            glm::mat4 getViewMatrix();
            glm::mat4 getProjMatrix();

            // TODO: possibly move these to Camera interface and
            // let 2D camera implement these methods as well
            void processKeyboard(CameraMovement direction, float deltaTime);
            void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
            void processMouseScroll(float yOffset);

            glm::vec3 getFront() { return front; }
        private:
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
            // Roll angle could be implemented for fligh simulator camera
            // Camera Options
            float movementSpeed;
            float mouseSensitivity;
    };
}

#endif // __Camera3D__
