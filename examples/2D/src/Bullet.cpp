#include "Bullet.hpp"

#include "ResourceManager.hpp"

using namespace Villain;

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, float sp, int life) {
    position = pos;
    direction = dir;
    speed = sp;
    lifeTime = life;
}

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

    glm::vec4 destRect = glm::vec4(position.x, position.y, 10, 10);
    spriteBatch.draw(destRect, uv, texture->getID(), 0.5f, color);
}

bool Bullet::update() {
    position += direction * speed;

    lifeTime--;
    if (lifeTime == 0)
        return true;

    return false;
}
