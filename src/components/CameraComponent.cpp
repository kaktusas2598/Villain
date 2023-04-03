#include "CameraComponent.hpp"

#include "../Camera2D.hpp"
#include "../Camera3D.hpp"

namespace Villain {

    void CameraComponent::handleInput(float deltaTime) {
        // TODO: implement Camera2D/ortho input logic here as well
        // Also would be useful to implement command pattern to abstract control keys and customize them
        Camera3D* cam3D = dynamic_cast<Camera3D*>(camera);
        if (cam3D != nullptr) {
            //FIXME: relative mode fixes camera restraint problem, but camera never stops moving for some reason
            if (!Engine::editModeActive()) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), 0, 0); // this seem to fix moving camera issue, but cursor disappears?

                // Also disable mouse in edit mode for now
                glm::vec2 mouseOffsets = TheInputManager::Instance()->getMouseOffsets();
                cam3D->processMouseMovement(mouseOffsets.x, mouseOffsets.y);
            }
            // TODO:
            //camera.processMouseScroll(yOffset);

            if (InputManager::Instance()->isKeyDown(SDLK_w)) {
                cam3D->processKeyboard(CameraMovement::FORWARD, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_s)) {
                cam3D->processKeyboard(CameraMovement::BACKWARD, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_a)) {
                cam3D->processKeyboard(CameraMovement::LEFT, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_d)) {
                cam3D->processKeyboard(CameraMovement::RIGHT, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
                cam3D->processKeyboard(CameraMovement::UP, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_LSHIFT)) {
                cam3D->processKeyboard(CameraMovement::DOWN, deltaTime);
            }
        }
    }

    void CameraComponent::addToEngine(Engine* engine) {
        // NOTE: bad solution once we have multiple cameras in place
        engine->getRenderingEngine()->setMainCamera(*camera);
    }
}
