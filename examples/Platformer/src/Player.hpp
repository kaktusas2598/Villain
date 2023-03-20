#ifndef __PLAYER__
#define __PLAYER__

#include "Box.hpp"
#include "SpriteBatch.hpp"

class Player {
    public:
        Player();
        ~Player();

        void init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, unsigned int texture, const glm::vec4& col);

        void draw(Villain::SpriteBatch& batch);
        void update();

        Box& getBox() { return collisionBox; }

    private:
        Box collisionBox;


};

#endif // __PLAYER__
