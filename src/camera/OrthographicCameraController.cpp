#include "OrthographicCameraController.hpp"

namespace Villain {

    void OrthographicCameraController::update(float deltaTime) {
        // In engine editor overlay, disable camera rotations
        if (!Engine::editModeActive()) {

            // Not sure if really need this, it hides the cursor but there are other methods to do that
            SDL_SetRelativeMouseMode(SDL_TRUE);
            // Warping mouse cursor in center for the infinite camera movement
            SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), Engine::getScreenWidth()/2, Engine::getScreenHeight()/2);

            glm::vec2 mouseOffsets = Input::Get()->getMouseOffsets();
            mouseOffsets *= sensitivity;

            float yaw = camera->getYaw();
            float pitch = camera->getPitch();
            yaw += mouseOffsets.x;
            pitch += mouseOffsets.y;

            // Prevent camera from flipping/ going underground
            if (pitch > 0.0f)
                pitch = 0.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            camera->setRotation({pitch, yaw, 0.0f});
        }

        camera->offsetScale(Input::Get()->mousewheel * deltaTime);

        if (moveControls) {
            if (Input::Get()->isKeyDown(SDLK_w)) {
                camera->setPosition(camera->getPosition() + camera->getFront() * movementSpeed);
            }
            if (Input::Get()->isKeyDown(SDLK_s)) {
                camera->setPosition(camera->getPosition() - camera->getFront() * movementSpeed);
            }
            if (Input::Get()->isKeyDown(SDLK_a)) {
                camera->setPosition(camera->getPosition() - camera->getRight() * movementSpeed);
            }
            if (Input::Get()->isKeyDown(SDLK_d)) {
                camera->setPosition(camera->getPosition() + camera->getRight() * movementSpeed);
            }
        }
    }
}
