#ifndef __Vertex__
#define __Vertex__

#include "VertexBufferLayout.hpp"
#include "glm/glm.hpp"

struct VertexP1 {
    glm::vec3 Position;
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        return layout;
    }
};

struct VertexP1N1 {
    glm::vec3 Position;
    glm::vec3 Normal;
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        return layout;
    }
};

struct VertexP1N1UV {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        return layout;
    }
};

struct VertexP1N1C1UV {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec4 Color;
    glm::vec2 UV;
};



// TODO: get rid of these
// Currently used for Model and Mesh
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
 };

// Currently used for sprite batch
struct VertexC {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 UV;
};

#endif // __Vertex__
