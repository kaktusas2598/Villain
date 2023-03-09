#ifndef __BULLET__
#define __BULLET__

#include "glm/glm.hpp"
#include "SpriteBatch.hpp"

class Human;
class Zombie;

// TODO: radius and size will all be needed for sprite collision/rendering
// so refactor these
const int BULLET_RADIUS = 5;

class Bullet {
    public:
        Bullet (glm::vec2 pos, glm::vec2 dir, float sp, int life);
        Bullet (glm::vec2 pos, glm::vec2 dir, float sp, int life, float dmg);
        ~Bullet ();

        void init(glm::vec2 pos, glm::vec2 dir, float sp, int life);
        void draw(Villain::SpriteBatch& spriteBatch);
        // True if needs to be destroyed
        bool update();
        bool update(std::vector<Human*>& humans, std::vector<Zombie*>& zombies);

    private:
        // Game options
        float damage;

        // Projectile options, might be refactored to engine
        glm::vec2 direction;
        glm::vec2 position;
        float speed;
        int lifeTime; // Lifetime in frames
};

#endif // __BULLET__
