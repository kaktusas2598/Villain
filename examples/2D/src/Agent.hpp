#ifndef __AGENT__
#define __AGENT__

#include "SpriteBatch.hpp"
#include "Texture.hpp"
#include "glm/glm.hpp"

#include "Level.hpp"

// Forward declaration to prevent circular includes
class Zombie;
class Human;

class Agent {
    public:
        Agent();
        virtual ~Agent();

        virtual void update(Villain::Level& level, std::vector<Human*>& humans, std::vector<Zombie*>& zombies) = 0;

        void collideWithLevel(Villain::Level& level);

        void draw(Villain::SpriteBatch& batch);

        glm::vec3 getPosition() const { return position; }
    protected:

        void checkTilePosition(Villain::Level& level, std::vector<glm::vec2>& collideTilePositions, float x, float y);
        void collideWithTile(glm::vec2 tilePosition, float tileSize);

        glm::vec3 position;
        float speed;

        Texture* texture;
        int currentRow, currentFrame; //<<< Current animation frame
        int frameSize;
        float scale;
};

#endif // __AGENT__
