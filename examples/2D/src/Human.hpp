#ifndef __HUMAN__
#define __HUMAN__

#include "Agent.hpp"

class Human : public Agent {
    public:
        Human();
        ~Human();

        void init(glm::vec3 pos, float sp, Texture* t);
        void update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);
    private:
        glm::vec2 direction;

};

#endif // __HUMAN__
