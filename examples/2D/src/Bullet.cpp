#include "Bullet.hpp"

#include "ResourceManager.hpp"

#include "Agent.hpp"

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

bool Bullet::update(Villain::Level& level) {
    position += direction * speed;

    lifeTime--;
    if (lifeTime == 0 || collideWithLevel(level))
        return true;

    return false;
}

bool Bullet::collideWithAgent(Agent* agent) {
    const float spriteRadius = 48.0f/2;//TODO: fix this hardcoded value in Agent
    glm::vec2 centrePositionA = glm::vec2(position);
    glm::vec2 centrePositionB = glm::vec2(agent->getPosition()) + glm::vec2(spriteRadius);
    const float minDistance = spriteRadius + BULLET_RADIUS;

    glm::vec2 distVec = centrePositionA - centrePositionB;

    float collisionDepth = minDistance - glm::length(distVec);
    // Agent collision!
    if (collisionDepth > 0) {
        return true;
    }

    return false;
}

// SImplest spherical collision
bool Bullet::collideWithLevel(Villain::Level& level) {
    std::vector<Villain::TileLayer*> layers = level.getCollidableLayers();
    float tileSize = layers[0]->getTileSize() * layers[0]->getScale();
    auto tileIds = layers[0]->getTileIDs();

    glm::ivec2 gridPosition = glm::ivec2(floor(position.x / tileSize), floor(position.y / tileSize));

    //if (gridPosition.x < layers[0]->getMapWidth() || gridPosition.x >= 0
            //|| gridPosition.y < layers[0]->getMapHeight() || gridPosition.y >= 0) {
        //return true;
    //}
    return tileIds[gridPosition.y][gridPosition.x] != 0;
}
