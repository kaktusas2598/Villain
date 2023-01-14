#ifndef __Vertex__
#define __Vertex__

#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    //glm::vec3 Color;
    glm::vec2 TexCoords;
};
#endif // __Vertex__
