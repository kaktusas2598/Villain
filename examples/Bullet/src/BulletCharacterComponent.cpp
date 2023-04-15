#include "BulletCharacterComponent.hpp"

#include "InputManager.hpp"
using namespace Villain;

void BulletCharacterComponent::handleInput(float deltaTime) {
    btVector3 front = btVector3(GetTransform()->getForward().x, GetTransform()->getForward().y, GetTransform()->getForward().z);
    btVector3 right = btVector3(GetTransform()->getRight().x, GetTransform()->getRight().y, GetTransform()->getRight().z);
    btVector3 up = btVector3(GetTransform()->getUp().x, GetTransform()->getUp().y, GetTransform()->getUp().z);

    if (InputManager::Instance()->isKeyDown(SDLK_w)) {
        controller->setMovementDirection(front);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_s)) {
        controller->setMovementDirection(-front);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_a)) {
        controller->setMovementDirection(-right);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_d)) {
        controller->setMovementDirection(right);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
        controller->jump(btVector3(0.0, 1.0, 0.0));
    }
    btVector3 origin = controller->getBody()->getWorldTransform().getOrigin();
    GetTransform()->setPos(glm::vec3(origin.getX(), origin.getY(), origin.getZ()));
}
