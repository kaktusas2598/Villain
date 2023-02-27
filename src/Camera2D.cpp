#include "Camera2D.hpp"

namespace Villain {

    Camera2D::Camera2D(): position(0.0f, 0.0f, 1.0f), zoom(1.0f), screenWidth(100.0f), screenHeight(100.0f) {
    }

    Camera2D::~Camera2D() {
    }

    void Camera2D::init(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, 0.1f, 100.0f);
    }

    glm::mat4 Camera2D::getCameraMatrix() {
        // Set ortohrapic projection and center camera around position vector
        glm::vec3 cameraTranslate(-position.x + screenWidth/2, -position.y + screenHeight/2, -position.z);
        glm::mat4 cameraMatrix = glm::translate(projection, cameraTranslate);
        // Camera zoom
        cameraMatrix = glm::scale(cameraMatrix, glm::vec3(zoom));

        return cameraMatrix;
    }


    glm::vec2 Camera2D::screenToWorld(glm::vec2 screenCoords) {
        // Make 0,0 the center
        screenCoords -= glm::vec2(screenWidth/2, screenHeight/2);
        // Apply camera scale
        screenCoords /= zoom;
        // translate
        screenCoords += glm::vec2(position.x, position.y);

        return screenCoords;
    }
}
