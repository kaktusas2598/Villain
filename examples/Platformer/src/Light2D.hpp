#ifndef __LIGHT_2D__
#define __LIGHT_2D__

#include "SpriteBatch.hpp"

// HACK:
class Light2D {
    public:
        void draw(Villain::SpriteBatch& batch) {
            glm::vec4 destRect;
            destRect.x = position.x - size / 2.0f;
            destRect.y = position.y - size / 2.0f;
            destRect.z = size;
            destRect.w = size;
            // Some kind of hacky way of lighting, calculated in shader and
            // drawn using this quad with some help from blending
            glm::vec4 uvRect(-1.0f, -1.0f, 2.0f, 2.0f);
            batch.draw(destRect, uvRect, 0, position.z, color, 0.0f);
        }

        glm::vec4 color;
        glm::vec3 position;
        float size;
};

#endif // __LIGHT_2D__
