#include "MoveController.hpp"

namespace Villain {

    void MoveController::handleInput(float deltaTime) {
        float velocity = speed * deltaTime;
        glm::vec3 translatePos = glm::vec3(0.0f);
        if (InputManager::Instance()->isKeyDown(SDLK_w)) {
            translatePos += GetTransform()->getForward() * velocity;
            //cam3D->processKeyboard(CameraMovement::FORWARD, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_s)) {
            translatePos -= GetTransform()->getForward() * velocity;
            //cam3D->processKeyboard(CameraMovement::BACKWARD, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_a)) {
            translatePos -= GetTransform()->getRight() * velocity;
            //cam3D->processKeyboard(CameraMovement::LEFT, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_d)) {
            translatePos += GetTransform()->getRight() * velocity;
            //cam3D->processKeyboard(CameraMovement::RIGHT, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
            translatePos += GetTransform()->getUp() * velocity;
            //cam3D->processKeyboard(CameraMovement::UP, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_LSHIFT)) {
            translatePos -= GetTransform()->getUp() * velocity;
            //cam3D->processKeyboard(CameraMovement::DOWN, deltaTime);
        }
        GetTransform()->translatePosition(translatePos);
    }
}
