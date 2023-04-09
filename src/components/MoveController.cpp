#include "MoveController.hpp"

namespace Villain {

    void MoveController::handleInput(float deltaTime) {
        float velocity = speed * deltaTime;
        glm::vec3 translatePos = glm::vec3(0.0f);
        glm::vec3 pos = GetTransform()->getPos();
        if (InputManager::Instance()->isKeyDown(SDLK_w)) {
            //pos.z -= distance;
            translatePos += GetTransform()->getForward() * velocity;
            //cam3D->processKeyboard(CameraMovement::FORWARD, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_s)) {
            //pos.z += distance;
            translatePos -= GetTransform()->getForward() * velocity;
            //cam3D->processKeyboard(CameraMovement::BACKWARD, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_a)) {
            //pos.x -= distance;
            translatePos -= GetTransform()->getRight() * velocity;
            //cam3D->processKeyboard(CameraMovement::LEFT, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_d)) {
            //pos.x += distance;
            translatePos += GetTransform()->getRight() * velocity;
            //cam3D->processKeyboard(CameraMovement::RIGHT, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
            //pos.y += distance;
            translatePos += GetTransform()->getUp() * velocity;
            //cam3D->processKeyboard(CameraMovement::UP, deltaTime);
        }
        if (InputManager::Instance()->isKeyDown(SDLK_LSHIFT)) {
            //pos.y -= distance;
            translatePos -= GetTransform()->getUp() * velocity;
            //cam3D->processKeyboard(CameraMovement::DOWN, deltaTime);
        }
        GetTransform()->translatePosition(translatePos);
        //GetTransform()->setPos(pos);
    }
}
