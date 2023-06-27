#include "Camera.hpp"

#include "Logger.hpp"

namespace Villain {

    Camera::Camera(ProjectionType projType) : projectionType(projType) {

        position = glm::vec3(0.0f, 0.0f, 1.0f);
        up = glm::vec3(0.0, 1.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        worldUp = up;
        switch (projectionType) {
            case ProjectionType::ORTHOGRAPHIC_2D:
                screenWidth = 100.0f;
                screenHeight = 100.0f;
                zoom = 1.0f;
                break;
            case ProjectionType::PERSPECTIVE:
                yaw = YAW;
                pitch = PITCH;
                movementSpeed = SPEED;
                mouseSensitivity = SENSITIVITY;
                zoom = 45.0f;
                updateCameraVectors();
                break;
            default:
                break;
        }
    }

    void Camera::rescale(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        // Recalculate projection matrix
        getProjMatrix();
    }

    glm::mat4 Camera::getViewMatrix() {
        switch (projectionType) {
            case ProjectionType::NONE:
                return glm::mat4(1.0f);
            case ProjectionType::PERSPECTIVE:
                return glm::lookAt(position, position + front, up);
            case ProjectionType::THIRD_PERSON:
                position = target - distanceToTarget * front;
                return glm::lookAt(position, target, up);
            case ProjectionType::ORTHOGRAPHIC:
                // TODO: test
                return glm::lookAt(position, position + front, up);
            case ProjectionType::ORTHOGRAPHIC_2D:
                // Center camera on the screen and apply zoom
                glm::vec3 cameraTranslate(-position.x + screenWidth/2.0f, -position.y + screenHeight/2.0f, -position.z);
                glm::mat4 cameraMatrix = glm::translate(glm::mat4(1.0f), cameraTranslate);
                return glm::scale(cameraMatrix, glm::vec3(zoom));
        }
        return glm::mat4(1.0f);
    }

    glm::mat4 Camera::getProjMatrix() {
        switch (projectionType) {
            case ProjectionType::NONE:
                projection = glm::mat4(1.0f);
                break;
            case ProjectionType::ORTHOGRAPHIC:
                projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, 0.1f, 100.0f);
                break;
            case ProjectionType::ORTHOGRAPHIC_2D:
                projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, 0.1f, 100.0f);
                break;
            case ProjectionType::PERSPECTIVE:
                projection = glm::perspective(glm::radians(zoom), (float)screenWidth/(float)screenHeight, zNear, zFar);
                break;
            case ProjectionType::THIRD_PERSON:
                projection = glm::perspective(glm::radians(zoom), (float)screenWidth/(float)screenHeight, zNear, zFar);
                break;

        }
        return projection;
    }


    glm::vec2 Camera::screenToWorld(glm::vec2 screenCoords) {
        if (projectionType != ProjectionType::ORTHOGRAPHIC_2D) {
            Logger::Instance()->error("Method not supported for this camera type\n");
        }
        // Invert y
        screenCoords.y = screenHeight - screenCoords.y;
        // Make 0,0 the center
        screenCoords -= glm::vec2(screenWidth/2, screenHeight/2);
        // Apply camera scale
        screenCoords /= zoom;
        // translate
        screenCoords += glm::vec2(position.x, position.y);

        return screenCoords;
    }

    glm::vec3 Camera::mouseRayToWorld(const glm::vec2& pos) {
        if (projectionType == ProjectionType::ORTHOGRAPHIC_2D) {
            Logger::Instance()->error("Use Camera::screenToWorld() instead\n");
        }
        // Reference: https://antongerdelan.net/opengl/raycasting.html
        // Normalised device coords
        float x = (2.0f * pos.x) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * pos.y) / screenHeight;
        // Homogeneous clip coords
        glm::vec4 rayClip = {x, y, -1.0f, 1.0f};
        // Camera/eye/view coords
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = {rayEye.x, rayEye.y, -1.0f, 0.0f};
        // World coords
        glm::vec3 rayWorld = glm::vec3(glm::inverse(this->getViewMatrix()) * rayEye);
        return glm::normalize(rayWorld);
    }

    // AABB test to see if quad is in camera's view space and needs to be rendered
    bool Camera::quadInView(const glm::vec2& pos, const glm::vec2& dimensions) {
        if (projectionType != ProjectionType::ORTHOGRAPHIC_2D) {
            Logger::Instance()->error("Method not supported for this camera type\n");
        }
        glm::vec2 scaledScreen = glm::vec2(screenWidth, screenHeight) / zoom;

        const float minDistanceX = dimensions.x / 2.0f + scaledScreen.x / 2.0f;
        const float minDistanceY = dimensions.y / 2.0f + scaledScreen.y / 2.0f;

        glm::vec2 centrePosition = pos + dimensions / 2.0f;
        glm::vec2 distance = centrePosition - glm::vec2(position);

        // Calculate collision penetration depth
        float xDepth = minDistanceX - abs(distance.x);
        float yDepth = minDistanceY - abs(distance.y);

        // Definitely collision!
        if (xDepth > 0 && yDepth > 0) {
            return true;
        }

        return false;
    }

    void Camera::offsetPosition(const glm::vec2& offset) {
        position.x += offset.x;
        position.y += offset.y;
    }

    void Camera::offsetScale(float offset) {
        zoom += offset;
        if (zoom < 0.001f)
            zoom = 0.001f;
    }


    Frustum Camera::getFrustum() {
        if (projectionType != ProjectionType::PERSPECTIVE) {
            Logger::Instance()->error("getFrustum(): Method not supported for this camera type\n");
            return Frustum();
        }
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

    void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
        // TODO: make projection-independant movement
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

    void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
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

    void Camera::processMouseScroll(float yOffset) {
        // TODO: merge with ortho 2d zoom
        zoom -= (float)yOffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

    void Camera::setZoom(float z) {
        zoom = z;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

    void Camera::updateCameraVectors() {
        // TODO: implement roll

        // Calculate new front vector using euler angles
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void Camera::setRotation(const glm::vec3& rotation) {
        pitch = rotation.x;
        yaw = rotation.y;
        roll = rotation.z;
        updateCameraVectors();
    }


}
