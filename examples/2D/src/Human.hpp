#ifndef __HUMAN__
#define __HUMAN__

#include "Agent.hpp"

class Human : public Agent {
    public:
        Human();
        ~Human();

        void init(glm::vec3 pos, float sp, Texture* t, float size = 48.0f, float sc = 1.0f);
        virtual void update(float deltaTime, Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);
    private:
        int frames;

};

#endif // __HUMAN__
