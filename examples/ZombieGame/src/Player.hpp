#ifndef __PLAYER__
#define __PLAYER__

#include "camera/Camera.hpp"
#include "Human.hpp"
#include "Bullet.hpp"

class Gun;

class Player : public Human {
    public:
        Player();
        ~Player();

        void addGun(Gun* gun);

        void init(glm::vec3 pos, float sp, Villain::Texture* t, Villain::Camera* cam, std::vector<Bullet>* pewpew, float size = 48.0f, float sc = 1.0f);
        void update(
                float deltaTime,
                Villain::Level& level,
                std::vector<Human*>& humans,
                std::vector<Zombie*>& zombies
                );
    private:
        std::vector<Gun*> guns;
        int currentGunIndex;
        Villain::Camera* camera;
        std::vector<Bullet>* bullets;

};

#endif // __PLAYER__
