#include "Bullet.hpp"

#include "ResourceManager.hpp"

using namespace Villain;

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, float sp, int life)
    : position(pos), direction(dir), speed(sp), lifeTime(life)
{}


Bullet::Bullet (glm::vec2 pos, glm::vec2 dir, float sp, int life, float dmg)
    : position(pos), direction(dir), speed(sp), lifeTime(life), damage(dmg)
{}

Bullet ::~Bullet () {}

void Bullet::init(glm::vec2 pos, glm::vec2 dir, float sp, int life) {
    position = pos;
    direction = dir;
    speed = sp;
    lifeTime = life;
}

void Bullet::draw(Villain::SpriteBatch& spriteBatch) {
    //TODO:bad practise to use static here, instead set texture in init or constructor
    static Texture* texture = ResourceManager::Instance()->loadTexture("assets/textures/bullet.png", "bullet");
    glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    glm::vec4 destRect = glm::vec4(position.x + BULLET_RADIUS, position.y + BULLET_RADIUS, BULLET_RADIUS * 2, BULLET_RADIUS * 2);
    // TODO: refactor color and depth to somewhere
    spriteBatch.draw(destRect, uv, texture->getID(), 0.5f, color);
}

bool Bullet::update() {
    position += direction * speed;

    lifeTime--;
    if (lifeTime == 0)
        return true;

    return false;
}

bool Bullet::update(std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {
    position += direction * speed;

    lifeTime--;
    if (lifeTime == 0)
        return true;

    return false;
}
