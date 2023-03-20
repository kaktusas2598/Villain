#ifndef __PLAYER__
#define __PLAYER__

#include "Capsule.hpp"
#include "DebugRenderer.hpp"
#include "SpriteBatch.hpp"

class Player {
    public:
        Player();
        ~Player();

        void init(
                b2World* world,
                const glm::vec2& position,
                const glm::vec2& drawDimensions,
                const glm::vec2& collisionDimensions,
                unsigned int texture,
                const glm::vec4& col);

        void draw(Villain::SpriteBatch& batch);
        void drawDebug(Villain::DebugRenderer& renderer);
        void update();

        Capsule& getCapsule() { return collisionShape; }

    private:
        Capsule collisionShape;

        unsigned int textureID;
        glm::vec4 color;
        glm::vec2 drawSize;
};

#endif // __PLAYER__
