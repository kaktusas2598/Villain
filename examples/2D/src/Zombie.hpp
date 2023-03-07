#ifndef __ZOMBIE__
#define __ZOMBIE__

#include "Agent.hpp"

class Zombie : public Agent {
    public:
        Zombie();
        ~Zombie();

        void init(glm::vec3 pos, float sp, Texture* t);
        void update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);

    private:

};

#endif // __ZOMBIE__
