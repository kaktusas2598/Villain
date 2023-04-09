#include "LookController.hpp"

namespace Villain {

    void LookController::handleInput(float deltaTime) {
        if (!Engine::editModeActive()) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), 0, 0); // this seem to fix moving camera issue, but cursor disappears?

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
