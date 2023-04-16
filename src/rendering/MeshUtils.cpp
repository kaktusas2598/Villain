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

    // TODO: need ability to specify different orientations, positions and sizes
    void MeshUtils::addTopFace(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices) {
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    }

    // TODO: UVs are assuming single texture used for all faces
    // Need to be able to specify size/position?- becomes AABB, not just a cube at that point, nice
    // Also 1 normal per face seems to work fine for basic shading
    void MeshUtils::addAABB(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec3& halfSize) {
        glm::vec3 minExtents = center - halfSize;
        glm::vec3 maxExtents = center + halfSize;
        // Front Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, maxExtents.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, maxExtents.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, maxExtents.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, maxExtents.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)});
        //Back Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, minExtents.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, minExtents.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, minExtents.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, minExtents.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
        // Top Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, minExtents.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, minExtents.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, maxExtents.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, maxExtents.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
        // Bottom Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, maxExtents.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, maxExtents.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, minExtents.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, minExtents.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
        // Left Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, maxExtents.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, maxExtents.y, minExtents.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, minExtents.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(minExtents.x, minExtents.y, maxExtents.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
        // Right Face
        addFace(indices, vertices->size(), false);
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, minExtents.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, maxExtents.y, maxExtents.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, maxExtents.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});
        vertices->push_back({glm::vec3(maxExtents.x, minExtents.y, minExtents.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
    }
}
