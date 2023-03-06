#ifndef __PLAYER__
#define __PLAYER__

#include "Human.hpp"

class Player : public Human {
    public:
        Player();
        ~Player();

        void init(glm::vec3 pos, float sp, Texture* t);
        void update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);
    private:

};

#endif // __PLAYER__
