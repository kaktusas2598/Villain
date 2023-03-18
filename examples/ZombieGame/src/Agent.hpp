#ifndef __AGENT__
#define __AGENT__

#include "SpriteBatch.hpp"
#include "Texture.hpp"
#include "glm/glm.hpp"

#include "Level.hpp"

// Forward declaration to prevent circular includes
class Zombie;
class Human;

// NOTE:A lot of stuff in this class can be refactor to the engine and used to make multiple apps:
// Sprite information: frame size, animation rows/columns, collider info(shape,margin etc.)
// Collision logic for Villain::Level to this new "Sprite" class/struct (Sprite class already exists though for rendering simple 2d sprites)
// eg.
//   Villain::Collision::collideSpriteLevel(Villain::Sprite* sprite, Vigilant::Level& level);
//   Villain::Collision::collideSprites(Villain::Sprite* spriteA, Vigilant::Sprite* spriteB);
class Agent {
    public:
        Agent();
        virtual ~Agent();

        virtual void update(float deltaTime, Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) = 0;

        bool collideWithLevel(Villain::Level& level);

        bool collideWithAgent(Agent* agent);

        void draw(Villain::SpriteBatch& batch);

        bool applyDamage(float dmg);
        float getHealth() const { return health; }

        glm::vec3 getPosition() const { return position; }
        void setPosition(glm::vec3 pos) { position = pos; }

        float getSize() const { return frameSize; }
    protected:

        void checkTilePosition(Villain::Level& level, std::vector<glm::vec2>& collideTilePositions, float x, float y);
        void collideWithTile(glm::vec2 tilePosition, float tileSize);

        glm::vec3 position;
        float speed;
        glm::vec2 direction = glm::vec2(1.0f, 0.0f);

        Texture* texture;
        int currentRow, currentFrame; //<<< Current animation frame
        float frameSize;
        float scale = 1.0f;

        float health;
};

#endif // __AGENT__
