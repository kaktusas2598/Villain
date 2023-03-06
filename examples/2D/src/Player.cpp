#include "Player.hpp"

#include "InputManager.hpp"

Player::Player()
{
}

Player::~Player()
{
}

void Player::init(glm::vec3 pos, float sp, Texture* t) {
    speed = sp;
    position = pos;
    texture = t;

}

// NOTE: probably would be better to pass InputManager as a ref here
void Player::update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_w))
        position.y += speed;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_a))
        position.x -= speed;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_s))
        position.y -= speed;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_d))
        position.x += speed;

    collideWithLevel(level);
}
