#ifndef __Vertex__
#define __Vertex__

/*
 * Defines different kind of Vertex types and each type of Vertex
 * implements getVertexLayout() method which setups vertex attribute pointers
 * essentially these structs can be used as template argument for template class
 */

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
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(4);
        layout.push<float>(2);
        return layout;
    }
};

struct VertexP1C1UV {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 UV;
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(4);
        layout.push<float>(2);
        return layout;
    }
};

#endif // __Vertex__
