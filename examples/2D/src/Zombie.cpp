#include "Zombie.hpp"

Zombie::Zombie()
{
}

Zombie::~Zombie()
{
}

void Zombie::init(glm::vec3 pos, float sp, Texture* t) {
    position = pos;
    speed = sp;
    texture = t;
}

void Zombie::update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {

}
