#include "Camera2D.hpp"
#include "glm/fwd.hpp"

namespace Villain {

    Camera2D::Camera2D(): position(0.0f, 0.0f, 1.0f), zoom(1.0f), screenWidth(100.0f), screenHeight(100.0f), needsMatrixUpdate(true) {
    }

    Camera2D::~Camera2D() {
    }

    void Camera2D::init(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, 0.1f, 100.0f);
    }

    void Camera2D::update() {
        if (needsMatrixUpdate) {
            projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, 0.1f, 100.0f);
            glm::vec3 cameraTranslate(-position.x + screenWidth/2.0f, -position.y + screenHeight/2.0f, -position.z);
            glm::mat4 cameraMatrix = glm::translate(projection, cameraTranslate);
            cameraMatrix = glm::scale(cameraMatrix, glm::vec3(zoom));
        }
    }

    glm::mat4 Camera2D::getCameraMatrix() {
        //glm::vec3 cameraTranslate(-position.x, -position.y, -position.z);
        // Set ortohrapic projection and center camera around position vector
        glm::vec3 cameraTranslate(-position.x + screenWidth/2.0f, -position.y + screenHeight/2.0f, -position.z);
        glm::mat4 cameraMatrix = glm::translate(projection, cameraTranslate);
        // Camera zoom
        cameraMatrix = glm::scale(cameraMatrix, glm::vec3(zoom));

        return cameraMatrix;
    }

    glm::vec2 Camera2D::screenToWorld(glm::vec2 screenCoords) {
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

    // AABB test to see if quad is in camera's view space and needs to be rendered
    bool Camera2D::quadInView(const glm::vec2& pos, const glm::vec2& dimensions) {
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
}
