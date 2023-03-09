#ifndef __HUMAN__
#define __HUMAN__

#include "Agent.hpp"

class Human : public Agent {
    public:
        Human();
        ~Human();

        void init(glm::vec3 pos, float sp, Texture* t);
        virtual void update(float deltaTime, Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);
    private:
        glm::vec2 direction;
        int frames;

};

#endif // __HUMAN__
