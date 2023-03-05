#ifndef __AGENT__
#define __AGENT__

#include "SpriteBatch.hpp"
#include "Texture.hpp"
#include "glm/glm.hpp"

class Agent {
    public:
        Agent();
        virtual ~Agent();

        virtual void update() = 0;

        void draw(Villain::SpriteBatch& batch);

        glm::vec3 getPosition() const { return position; }
    protected:
        glm::vec3 position;
        float speed;
        Texture* texture;
};

#endif // __AGENT__
