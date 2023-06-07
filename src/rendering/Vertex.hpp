#ifndef __Vertex__
#define __Vertex__

#define MAX_BONE_INFLUENCE 4

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

struct VertexP1UV {
    glm::vec3 Position;
    glm::vec2 UV;
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(2);
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

struct VertexP1N1T1B1UV {
    glm::vec3 Position;
    glm::vec3 Normal;
    // Texture coordinates
    glm::vec2 UV;
    // Tangent and BiTangent used to calculate tangent space and realistic lighting
    glm::vec3 Tangent;
    glm::vec3 BiTangent;
    // Bones which will influence this vertex
    int BoneIDs[MAX_BONE_INFLUENCE];
    // Weights from each bone
    float Weights[MAX_BONE_INFLUENCE];
    static VertexBufferLayout getVertexLayout() {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        layout.push<float>(3);
        layout.push<float>(3);

        layout.push<int>(4);
        layout.push<float>(4);
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
