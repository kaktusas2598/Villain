#include "MeshUtils.hpp"

namespace Villain {

    void MeshUtils::addFace(std::vector<unsigned int>* indices, int startLocation, bool direction) {
        if (direction) {
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 3);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 0);
        } else {
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 2);//0
            indices->push_back(startLocation + 3);//2
            indices->push_back(startLocation + 0);//3
        }
    }

    void MeshUtils::addTopFace(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices) {
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    }

    // TODO: UVs are assuming single texture used for all faces
    // Need to be able to specify size/position?
    // Also 1 normal per face seems to work fine for basic shading
    void MeshUtils::addCube(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec3& size) {
        // Front Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)});
        //Back Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
        // Top Face
        addTopFace(vertices, indices);
        // Bottom Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
        // Left Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
        // Right Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    }
}
