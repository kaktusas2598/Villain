#ifndef __Vertex__
#define __Vertex__

#include "glm/glm.hpp"

// Currently used for Model and Mesh
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

// Currently used for sprite batch
struct VertexC {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 UV;
    //glm::vec3 Normal;
};

#endif // __Vertex__
