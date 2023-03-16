#include "Player.hpp"

#include "InputManager.hpp"
#include "Gun.hpp"

Player::Player() : currentGunIndex(-1)
{}

Player::~Player() {}


void Player::addGun(Gun* gun) {
    guns.push_back(gun);

    // If no gun equipped, equip
    if (currentGunIndex == -1) {
        currentGunIndex = 0;
    }
}

void Player::init(glm::vec3 pos, float sp, Texture* t, Villain::Camera2D* cam, std::vector<Bullet>* pewpew, float size, float sc) {
    speed = sp;
    position = pos;
    texture = t;
    camera = cam;
    bullets = pewpew;
    health = 100;
    frameSize = size;
    scale = sc;
}

// NOTE: probably would be better to pass InputManager as a ref here
void Player::update(
                float deltaTime,
                Villain::Level& level,
                std::vector<Human*>& humans,
                std::vector<Zombie*>& zombies) {

    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_w))
        position.y += speed * deltaTime;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_a))
        position.x -= speed * deltaTime;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_s))
        position.y -= speed * deltaTime;
    if(Villain::TheInputManager::Instance()->isKeyDown(SDLK_d))
        position.x += speed * deltaTime;

    if(Villain::TheInputManager::Instance()->isKeyPressed(SDLK_1) && guns.size() >= 0) {
        std::cout << "Gun 0 selected\n";
        currentGunIndex = 0;
    }
    if(Villain::TheInputManager::Instance()->isKeyPressed(SDLK_2) && guns.size() >= 1) {
        std::cout << "Gun 1 selected\n";
        currentGunIndex = 1;
    }
    if(Villain::TheInputManager::Instance()->isKeyPressed(SDLK_3) && guns.size() >= 2) {
        std::cout << "Gun 2 selected\n";
        currentGunIndex = 2;
    }

    glm::vec2 mouseCoords = Villain::InputManager::Instance()->getMouseCoords();
    mouseCoords = camera->screenToWorld(mouseCoords);

    glm::vec2 centerPosition = glm::vec2(position) + glm::vec2(scale * frameSize / 2);
    direction = glm::normalize(mouseCoords - centerPosition);

    if (currentGunIndex != -1) {

        guns[currentGunIndex]->update(
                deltaTime,
                Villain::TheInputManager::Instance()->isKeyDown(SDL_BUTTON_LEFT),
                centerPosition,
                direction,
                *bullets);
    }

    collideWithLevel(level);
}
