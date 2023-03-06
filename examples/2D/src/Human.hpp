#ifndef __HUMAN__
#define __HUMAN__

#include "Agent.hpp"

class Human : public Agent {
    public:
        Human();
        ~Human();

        void update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies);
    private:

};

#endif // __HUMAN__
