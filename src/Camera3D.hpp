#ifndef __Camera3D__
#define __Camera3D__

#include "Camera.hpp"

namespace Villain {

    // TODO: Camera/Camera2D and Camera3D should be refactor to same class having Transform and option to choose
    // any type of projection and custom movement controls
    //
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
            glm::vec3 getRight() { return right; }
            glm::vec3 getUp() { return up; }
            glm::vec3 getRotation() { return glm::vec3(pitch, yaw, 0.0f); }
            float getZnear() { return zNear; }
            float getZfar() { return zFar; }
            void setZPlanes(float near, float far) { zNear = near, zFar = far; }
            void setRotation(const glm::vec3& rotation);

            Frustum getFrustum() {
                Frustum     frustum;
                const float halfVSide = zFar * tanf(glm::radians(zoom) * .5f);
                const float halfHSide = halfVSide * getAspectRatio();
                const glm::vec3 frontMultFar = zFar * front;

                frustum.nearFace = { position + zNear * front, front };
                frustum.farFace = { position + frontMultFar, -front };
                frustum.rightFace = { position, glm::cross(frontMultFar - right * halfHSide, up) };
                frustum.leftFace = { position, glm::cross(up, frontMultFar + right * halfHSide) };
                frustum.topFace = { position, glm::cross(right, frontMultFar - up * halfVSide) };
                frustum.bottomFace = { position, glm::cross(frontMultFar + up * halfVSide, right) };
                return frustum;
            }
            // HACK: hacked property for shadow map framebuffer
            void setNoProjection(bool noProj = true) { noProjection = noProj; }
        private:
            bool noProjection = false;
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
            float zNear = 0.1f;
            float zFar = 100.0f;
    };
}

#endif // __Camera3D__
