#include "Human.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include <random>

Human::Human() : frames(0)
{
}

Human::~Human()
{
}

void Human::init(glm::vec3 pos, float sp, Texture* t) {

    static std::mt19937 rndEngine(time(nullptr));
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    position = pos;
    speed = sp;
    texture = t;
    // Randomize human/npc direction
    direction = glm::vec2(dist(rndEngine), dist(rndEngine));
    // Make sure direction isn't zero
    if (direction.length() == 0) direction = glm::vec2(1.0f, 0.0f);
    direction = glm::normalize(direction);
}

void Human::update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) {

    position.x += direction.x * speed;
    position.y += direction.y * speed;

    static std::mt19937 rndEngine(time(nullptr));
    // Change distribution range for more wild direction changes!
    static std::uniform_real_distribution<float> dist(-.5f, .5f);
    if (frames == 20) {
        direction = glm::rotate(direction, dist(rndEngine));
        frames = 0;
    } else {
        frames++;
    }

    collideWithLevel(level);
}
