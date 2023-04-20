#include "LookController.hpp"

namespace Villain {

    void LookController::handleInput(float deltaTime) {
        if (!Engine::editModeActive()) {

            // Not sure if really need this, it hides the cursor but there are other methods to do that
            SDL_SetRelativeMouseMode(SDL_TRUE);
            // Warping mouse cursor in center for the infinite camera movement
            SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), Engine::getScreenWidth()/2, Engine::getScreenHeight()/2);

            // Also disable mouse in edit mode for now
            glm::vec2 mouseOffsets = TheInputManager::Instance()->getMouseOffsets();

            // TODO: process mouse wheel here as well?

            mouseOffsets.x *= sensitivity;
            mouseOffsets.y *= sensitivity;

            float yaw = GetTransform()->getEulerRot().y;
            float pitch = GetTransform()->getEulerRot().x;
            yaw += mouseOffsets.x;
            pitch += mouseOffsets.y;

            // Prevent camera from flipping
            if (constrainPitch) {
                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;
            }

            GetTransform()->setEulerRot(pitch, yaw, 0.f);
        }
    }
}
