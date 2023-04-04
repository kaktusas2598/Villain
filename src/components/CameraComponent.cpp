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
                input = true;
                cam3D->processKeyboard(CameraMovement::FORWARD, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_s)) {
                input = true;
                cam3D->processKeyboard(CameraMovement::BACKWARD, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_a)) {
                input = true;
                cam3D->processKeyboard(CameraMovement::LEFT, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_d)) {
                input = true;
                cam3D->processKeyboard(CameraMovement::RIGHT, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
                input = true;
                cam3D->processKeyboard(CameraMovement::UP, deltaTime);
            }
            if (InputManager::Instance()->isKeyDown(SDLK_LSHIFT)) {
                input = true;
                cam3D->processKeyboard(CameraMovement::DOWN, deltaTime);
            }

            // HACK: Not a great solution, but idea is this - if user moved camera's position, it will update paren'ts transform
            // but if no input was detected, camera will use transform's pos
            // NOTE: Potentially could be solved by refactoring Camera to use Transform class and make both Node and Camera have Transform
            // pointer pointing to the same address
            if (input)
                GetTransform()->setPos(cam3D->getPosition());
            else
                cam3D->setPosition(GetTransform()->getPos());

            input = false;
        }
    }

    void CameraComponent::addToEngine(Engine* engine) {
        // NOTE: bad solution once we have multiple cameras in place
        engine->getRenderingEngine()->setMainCamera(*camera);
    }
}
