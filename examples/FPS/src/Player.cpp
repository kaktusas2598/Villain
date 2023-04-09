#include "Player.hpp"

#include "InputManager.hpp"

using namespace Villain;

// Custom Move Controller for this game, locked on y height
void Player::handleInput(float deltaTime) {
    const float speed = 5.0f;
    float velocity = speed * deltaTime;
        glm::vec3 translatePos = glm::vec3(0.0f);
        if (InputManager::Instance()->isKeyDown(SDLK_w)) {
            translatePos += GetTransform()->getForward() * velocity;
        }
        if (InputManager::Instance()->isKeyDown(SDLK_s)) {
            translatePos -= GetTransform()->getForward() * velocity;
        }
        if (InputManager::Instance()->isKeyDown(SDLK_a)) {
            translatePos -= GetTransform()->getRight() * velocity;
        }
        if (InputManager::Instance()->isKeyDown(SDLK_d)) {
            translatePos += GetTransform()->getRight() * velocity;
        }
        GetTransform()->translatePosition(translatePos);
}

void Player::update(float deltaTime) {

}

