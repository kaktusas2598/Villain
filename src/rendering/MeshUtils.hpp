#ifndef __MESH_UTILS__
#define __MESH_UTILS__

#include "Vertex.hpp"
#include <vector>

namespace Villain {


    // Various utility methods to generate vertex data, primitives
    // NOTE:Not all Vertex types can be used, like it wouldn;t make sense to generate tangents
    // for simple debug vertices, so maybe class should not be template and instead some methods may be, but
    // then they need to be declared in header
    template <class VertexType> class MeshUtils {
        public:

        /*
         * Generate triangle indices for a quad
         *
         * @param int startLocation - starting vertex location/index
         */
        static void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);

        // Generate tangents and bitangents for a mesh
        static void addTangents(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices);

        // Calculate normal for a triangle
        static glm::vec3 getNormal(const::glm::vec3& i1, const::glm::vec3& i2, const::glm::vec3& i3);
        // Calculate tangents and bitangents for a triangle, positions and UVs required
        static void getTangents(VertexP1N1T1B1UV& i1, VertexP1N1T1B1UV& i2, VertexP1N1T1B1UV& i3);

        // TODO:
        //template <class VertexType>
        //static void addQuad(std::vector<VertexType>* vertices);

        // NOTE: Need a good way to generate a face in any orientation, at the very least Axis-Aligned
        static void addXZPlane(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec2& halfSize = glm::vec3(0.5f),
                float* uvCoords = defaultUVMap,
                bool direction = false
                );

        static void addXYPlane(
                std::vector<VertexType>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec2& halfSize = glm::vec3(0.5f),
                float* uvCoords = defaultUVMap,
                bool direction = false
                );

        static void addYZPlane(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec2& halfSize = glm::vec3(0.5f),
                float* uvCoords = defaultUVMap,
                bool direction = false
                );

        // Setup vertices and indices for 3D Axis-Aligned Bounding Box, by default 1f sized cube centered around the origin
        static void addAABB(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec3& halfSize = glm::vec3(0.5f));

        // Generate sphere positions, normals and UVs
        static void addSphere(
                std::vector<VertexType>* vertices,
                std::vector<unsigned int>* indices,
                float radius,
                const glm::vec3& center = glm::vec3(0.0f));

        private:
        // Min X(S), Max X(S), MIN Y(T), MAX Y(T)
        static float defaultUVMap[4];
    };
}

#endif // __MESH_UTILS__
