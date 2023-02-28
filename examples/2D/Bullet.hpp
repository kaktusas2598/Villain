#ifndef __BULLET__
#define __BULLET__

#include "glm/glm.hpp"
#include "SpriteBatch.hpp"

class Bullet {
    public:
        Bullet (glm::vec2 pos, glm::vec2 dir, float sp, int life);
        ~Bullet ();

        void init(glm::vec2 pos, glm::vec2 dir, float sp, int life);
        void draw(Villain::SpriteBatch& spriteBatch);
        // True if needs to be destroyed
        bool update();

    private:
        glm::vec2 direction;
        glm::vec2 position;
        float speed;
        int lifeTime; // Lifetime in frames
};

#endif // __BULLET__
