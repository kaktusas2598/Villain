#ifndef __GUN__
#define __GUN__

#include <string>
#include <vector>
#include "Bullet.hpp"

class Gun {
    public:
        Gun(std::string title, int rate, int bps, float spr, float bulletSp, float bulletDmg, int bulletLife);
        ~Gun();

        void update(bool mouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);
    private:
        void fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);

        std::string name;
        int fireRate; //<<< fire rate in terms of frames
        int bulletsPerShot;
        float spread; //<<< accuracy

        float bulletSpeed;
        float bulletDamage;
        int bulletLifetime;

        int frameCounter;
};

#endif // __GUN__
