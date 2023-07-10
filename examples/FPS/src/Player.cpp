#include "Player.hpp"

#include "Input.hpp"

using namespace Villain;

const float Player::PLAYER_SIZE = 0.3f;
const float SHOOT_DISTANCE = 100.f;

// Custom Move Controller for this game, locked on y height
void Player::handleInput(float deltaTime) {
    // NOTE: Commented out Movement logic below, because all we need to do here is lock y height and then we can use
    // generic Look and Move controllers, but the problem is that MoveController reacts to space/lshift, need
    // ability to override that, so using our move logix for now
    movement = glm::vec3(0.0f);
    if (Input::Get()->isKeyDown(SDLK_w)) {
        movement += GetTransform()->getForward();
    }
    if (Input::Get()->isKeyDown(SDLK_s)) {
        movement -= GetTransform()->getForward();
    }
    if (Input::Get()->isKeyDown(SDLK_a)) {
        movement -= GetTransform()->getRight();
    }
    if (Input::Get()->isKeyDown(SDLK_d)) {
        movement += GetTransform()->getRight();
    }

    if (Input::Get()->isKeyDown(SDLK_e)) {
        // TODO: only pass true when all monsters are killed
        currentLevel->openDoors(GetTransform()->getPos(), true);
    }

    if (Input::Get()->isKeyPressed(SDL_BUTTON_LEFT)) {
        glm::vec2 lineStart = glm::vec2(GetTransform()->getPos().x, GetTransform()->getPos().z);
        // Needed camera's front vector here, but playuer's should be the same
        glm::vec2 castDirection = glm::normalize(glm::vec2(GetTransform()->getForward().x, GetTransform()->getForward().z));
        //std::cout << "Camera front: " << castDirection.x << ", " << castDirection.y << "\n";
        glm::vec2 lineEnd = lineStart + castDirection * SHOOT_DISTANCE;

        currentLevel->checkIntersections(lineStart, lineEnd, true);
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

void Player::damage(int amount) {
    health -= amount;

    if (health <= 0) {
        std::cout << "GAME OVER\n";
        Engine::setRunning(false);
    }
}

