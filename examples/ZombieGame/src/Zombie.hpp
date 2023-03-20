#ifndef __ZOMBIE__
#define __ZOMBIE__

#include "Agent.hpp"

class Zombie : public Agent {
    public:
        Zombie();
        ~Zombie();

        void init(glm::vec3 pos, float sp, Villain::Texture* t, float size = 32.0f, float sc = 1.0f);
        void update(float deltaTime, Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) override;

    private:
        Human* getNearestHuman(std::vector<Human*>& humans);
};

#endif // __ZOMBIE__
