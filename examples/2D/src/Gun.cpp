#include "Gun.hpp"
#include "SoundManager.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <random>
#include <ctime>

Gun::Gun(std::string title, int rate, int bps, float spr, float bulletSp, float bulletDmg, int bulletLife, std::string shotSound) :
    name(title),
    fireRate(rate),
    bulletsPerShot(bps),
    spread(spr),
    bulletSpeed(bulletSp),
    bulletDamage(bulletDmg),
    bulletLifetime(bulletLife),
    frameCounter(0),
    sound(shotSound)
{}

Gun::~Gun()
{}


void Gun::update(float deltaTime, bool mouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets) {
    frameCounter += 10.0f * deltaTime;
    if (frameCounter >= fireRate && mouseDown) {
        fire(position, direction, bullets);
        frameCounter = 0;
    }
}

void Gun::fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets) {
    // NOTE: on Windows, time() requires including ctime
    static std::mt19937 rndEngine(time(nullptr));
    static std::uniform_real_distribution<float> dist(-spread, spread);

    Villain::SoundManager::Instance()->playSound(sound);

    for (int i = 0; i < bulletsPerShot; i++) {
        bullets.emplace_back(position, glm::rotate(direction, glm::radians(dist(rndEngine))), bulletSpeed, bulletLifetime, bulletDamage);
    }
}

