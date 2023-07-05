#ifndef __Camera__
#define __Camera__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "CameraController.hpp"
#include "Frustum.hpp"
#include "NodeComponent.hpp"
#include "Transform.hpp"
#include <memory>

namespace Villain {

    class Engine;

    enum class CameraType {
        NONE,
        ORTHOGRAPHIC,
        ORTHOGRAPHIC_2D,
        FIRST_PERSON, // Or just basic perspective
        THIRD_PERSON
    };

    // Default values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;

    class Camera: public NodeComponent {
        public:
            Camera(CameraType cameraType = CameraType::FIRST_PERSON);
            ~Camera() {}

            virtual void update(float deltaTime);
            virtual void addToEngine(Engine* engine);

            // Call after creation and on window size change to change projection
            void rescale(int width, int height);

            glm::mat4 getViewMatrix();
            glm::mat4 getProjMatrix();

            glm::vec3& getPosition() { return position; }
            float getZoom() { return zoom; }
            float getAspectRatio() { return (float)screenWidth / (float)screenHeight; }
            float getZnear() { return zNear; }
            float getZfar() { return zFar; }

            float getYaw() const { return yaw; }
            float getPitch() const { return pitch; }
            float getRoll() const { return roll; }

            CameraType getType() { return type; }
            void setType(CameraType cameraType) { type = cameraType; switchCameraController(type); }

            void setPosition(const glm::vec3& newPos) { position = newPos; }
            void setZoom(float z);
            void setZPlanes(float near, float far) { zNear = near, zFar = far; }
            void setZNear(float near) { zNear = near; }
            void setZFar(float far) { zFar = far; }

            // TODO: need more testing, maybe try implementing for other projection types
            Frustum getFrustum();

            // TODO: TEST
            glm::vec3 mouseRayToWorld(const glm::vec2& pos);
            // ORTHOGRAPHIC_2D version
            glm::vec2 screenToWorld(glm::vec2 screenCoords);

            // Checks if quad is in Camera's clip space (2D camera only ATM)
            bool quadInView(const glm::vec2& pos, const glm::vec2& dimensions);

            void offsetPosition(const glm::vec2& offset);
            void offsetScale(float offset);
            void setMovementSpeed(float speed) { movementSpeed = speed; }
            void setSensitivity(float sens) { mouseSensitivity = sens; }

            glm::vec3 getFront() { return front; }
            glm::vec3 getRight() { return right; }
            glm::vec3 getUp() { return up; }
            glm::vec3 getRotation() { return glm::vec3(pitch, yaw, roll); }
            void setRotation(const glm::vec3& rotation);

            // 3RD Person Camera Specific
            void setTarget(Transform* t) { target = t; }
            void setDistanceToTarget(float distance) { distanceToTarget = distance; }
            float getDistanceToTarget() const { return distanceToTarget; }
            float getAngleAroundTarget() const { return angleAroundTarget; }

        private:
            int screenWidth, screenHeight;
            float zoom;
            glm::vec3 position;
            CameraType type;
            std::unique_ptr<CameraController> activeController;
            glm::mat4 projection;
            float zNear = 0.1f;
            float zFar = 100.0f;

            void switchCameraController(CameraType t);

            // Optional for 3rd person camera
            Transform* target = nullptr;
            float distanceToTarget = 10;
            float angleAroundTarget = 0;

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
