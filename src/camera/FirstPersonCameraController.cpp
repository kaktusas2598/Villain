#include "FirstPersonCameraController.hpp"

namespace Villain {

    void FirstPersonCameraController::update(float deltaTime) {
        if (Engine::editModeActive()) return;

        // Not sure if really need this, it hides the cursor but there are other methods to do that
        SDL_SetRelativeMouseMode(SDL_TRUE);
        // Warping mouse cursor in center for the infinite camera movement
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), Engine::getScreenWidth()/2, Engine::getScreenHeight()/2);

        // Also disable mouse in edit mode for now
        glm::vec2 mouseOffsets = TheInputManager::Instance()->getMouseOffsets();

        mouseOffsets.x *= sensitivity;
        mouseOffsets.y *= sensitivity;

        float yaw = camera->getYaw();
        float pitch = camera->getPitch();
        yaw += mouseOffsets.x;
        pitch += mouseOffsets.y;

        //// Prevent camera from flipping
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        camera->setRotation({pitch, yaw, 0.0f});

        camera->setZoom(camera->getZoom() + InputManager::Instance()->mousewheel);

        // TODO: movement logic
        camera->setPosition(camera->GetTransform()->getPos());
    }
}
