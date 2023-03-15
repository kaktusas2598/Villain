#include "Zombie.hpp"

#include "Human.hpp"

Zombie::Zombie() {}

Zombie::~Zombie()
{
}

void Zombie::init(glm::vec3 pos, float sp, Texture* t, float size, float sc) {
    position = pos;
    speed = sp;
    texture = t;
    frameSize = size;
    scale = sc;

    health = 100;
}

void Zombie::update(float deltaTime, Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {

    Human* closestHuman = getNearestHuman(humans);
    if (closestHuman != nullptr) {
        direction = glm::normalize(glm::vec2(closestHuman->getPosition()) - glm::vec2(position));
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
    }

    if (collideWithLevel(level)) {
    }

}


Human* Zombie::getNearestHuman(std::vector<Human*>& humans) {
    Human* closestHuman = nullptr;
    float smallestDistance = 999999.9f;
    for (int i = 0; i < humans.size(); i++) {
        glm::vec2 distVec = glm::vec2(humans[i]->getPosition()) - glm::vec2(position);
        float distance = glm::length(distVec);

        if (distance < smallestDistance) {
            smallestDistance = distance;
            closestHuman = humans[i];
        }
    }

    return closestHuman;
}
