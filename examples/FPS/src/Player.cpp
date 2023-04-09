#include "Player.hpp"

#include "InputManager.hpp"

using namespace Villain;

// Custom Move Controller for this game, locked on y height
void Player::handleInput(float deltaTime) {
    // NOTE: Commented out Movement logic below, because all we need to do here is lock y height and then we can use
    // generic Look and Move controllers, but the problem is that MoveController reacts to space/lshift, need
    // ability to override that
    glm::vec3 pos = GetTransform()->getPos();
    pos.y = 1.0f;
    GetTransform()->setPos(pos);

    //const float speed = 5.0f;
    //float velocity = speed * deltaTime;
    //glm::vec3 translatePos = glm::vec3(0.0f);
    //if (InputManager::Instance()->isKeyDown(SDLK_w)) {
        //translatePos += GetTransform()->getForward() * velocity;
    //}
    //if (InputManager::Instance()->isKeyDown(SDLK_s)) {
        //translatePos -= GetTransform()->getForward() * velocity;
    //}
    //if (InputManager::Instance()->isKeyDown(SDLK_a)) {
        //translatePos -= GetTransform()->getRight() * velocity;
    //}
    //if (InputManager::Instance()->isKeyDown(SDLK_d)) {
        //translatePos += GetTransform()->getRight() * velocity;
    //}
    // Lock movement on y axis to stay at the same height
    //translatePos.y = 0.0f;
    //GetTransform()->translatePosition(translatePos);
}

void Player::update(float deltaTime) {
}

