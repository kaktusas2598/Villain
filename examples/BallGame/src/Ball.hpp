#ifndef __BALL__
#define __BALL__

# include "glm/glm.hpp"

struct Cell;

struct Ball {

    Ball(float radius, float mass, const glm::vec2& pos, const glm::vec2& vel,
            unsigned int texture, const glm::vec4& color);

    float radius;
    float mass;
    glm::vec2 position;
    glm::vec2 velocity;
    unsigned int textureID = 0;
    glm::vec4 color;
    Cell* ownerCell = nullptr;
    int cellVectorIndex = -1;
};

#endif // __BALL__
