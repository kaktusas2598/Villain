#include "Camera3D.hpp"

namespace Villain {

    Camera3D::Camera3D(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->zoom = 45.0f;
        updateCameraVectors();
    }

    void Camera3D::rescale(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        projection = glm::perspective(glm::radians(zoom), (float)screenWidth/(float)screenHeight, zNear, zFar);
    }

    glm::mat4 Camera3D::getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 Camera3D::getProjMatrix() {
        // First param - FOV could be changed for zooming effect
        // 2nd param - aspect ratio
        // 3rd and 4th params - near and far planes
        projection = glm::perspective(glm::radians(zoom), (float)screenWidth/(float)screenHeight, zNear, zFar);
        return projection;
    }

    void Camera3D::processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        switch(direction) {
            case CameraMovement::FORWARD:
                position += front * velocity;
                break;
            case CameraMovement::BACKWARD:
                position -= front * velocity;
                break;
            case CameraMovement::LEFT:
                position -= right * velocity;
                break;
            case CameraMovement::RIGHT:
                position += right * velocity;
                break;
            case CameraMovement::UP:
                position.y += velocity;
                break;
            case CameraMovement::DOWN:
                position.y -= velocity;
                break;

            default:
                break;
        }
    }

    void Camera3D::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        yaw += xOffset;
        pitch += yOffset;

        // Prevent camera from flipping
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void Camera3D::processMouseScroll(float yOffset) {
        zoom -= (float)yOffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

    void Camera3D::updateCameraVectors() {
        // Calculate new front vector using euler angles
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void Camera3D::setRotation(const glm::vec3& rotation) {
        pitch = rotation.x;
        yaw = rotation.y;
        updateCameraVectors();
    }
}
