#ifndef __PLAYER__
#define __PLAYER__

#include "Camera2D.hpp"
#include "Human.hpp"
#include "Bullet.hpp"

class Gun;

class Player : public Human {
    public:
        Player();
        ~Player();

        void addGun(Gun* gun);

        void init(glm::vec3 pos, float sp, Texture* t, Villain::Camera2D* cam, std::vector<Bullet>* pewpew, float size = 48.0f);
        void update(
                float deltaTime,
                Villain::Level& level,
                std::vector<Human*>& humans,
                std::vector<Zombie*>& zombies
                );
    private:
        std::vector<Gun*> guns;
        int currentGunIndex;
        Villain::Camera2D* camera;
        std::vector<Bullet>* bullets;

};

#endif // __PLAYER__
