#include "Player.hpp"

#include "InputManager.hpp"

using namespace Villain;

const float PLAYER_SIZE = 0.3f;
const float OPEN_DISTANCE = 2.0f;

// Custom Move Controller for this game, locked on y height
void Player::handleInput(float deltaTime) {
    // NOTE: Commented out Movement logic below, because all we need to do here is lock y height and then we can use
    // generic Look and Move controllers, but the problem is that MoveController reacts to space/lshift, need
    // ability to override that, so using our move logix for now
    movement = glm::vec3(0.0f);
    if (InputManager::Instance()->isKeyDown(SDLK_w)) {
        movement += GetTransform()->getForward();
    }
    if (InputManager::Instance()->isKeyDown(SDLK_s)) {
        movement -= GetTransform()->getForward();
    }
    if (InputManager::Instance()->isKeyDown(SDLK_a)) {
        movement -= GetTransform()->getRight();
    }
    if (InputManager::Instance()->isKeyDown(SDLK_d)) {
        movement += GetTransform()->getRight();
    }

    if (InputManager::Instance()->isKeyDown(SDLK_e)) {
        for(auto& door : currentLevel->getDoors()) {
            float distanceToDoor = glm::length(door->GetTransform()->getPos() - GetTransform()->getPos());
            if (distanceToDoor < OPEN_DISTANCE) {
                door->open();
            }
        }
    }
}

void Player::update(float deltaTime) {
    //Lock movement on y axis to stay at the same height
    movement.y = 0.0f;
    float velocity = speed * deltaTime;

    glm::vec3 oldPos = GetTransform()->getPos();
    glm::vec3 newPos = oldPos + (movement * velocity);

    glm::vec3 collision = currentLevel->checkCollisions(oldPos, newPos, PLAYER_SIZE, PLAYER_SIZE);
    // Collision vector will have 1.f if we can move on that axis or 0.f if we can't
    movement.x *= collision.x;
    movement.z *= collision.z;

    GetTransform()->translatePosition(movement * speed * deltaTime);
}

