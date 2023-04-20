#ifndef __PLAYER__
#define __PLAYER__

#include "Capsule.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/SpriteBatch.hpp"
#include "rendering/TileAtlas.hpp"

enum class PlayerMoveState { STANDING, RUNNING, PUNCHING, JUMPING};

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

        Villain::TileAtlas spriteSheet;
        glm::vec4 color;
        glm::vec2 drawSize;

        bool onGround = false;
        bool punching = false;
        int direction = 1; // 1 or -1
        float animationTime;
        PlayerMoveState moveState = PlayerMoveState::STANDING;
};

#endif // __PLAYER__
