#ifndef __MESH_UTILS__
#define __MESH_UTILS__

#include "Vertex.hpp"
#include <vector>

namespace Villain {


    // Various utility methods to generate vertex data, primitives
    class MeshUtils {
        public:

        /*
         * Generate triangle indices for a quad
         *
         * @param int startLocation - starting vertex location/index
         */
        static void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);

        // Calculate normal from a triangle
        static glm::vec3 getNormal(const::glm::vec3& i1, const::glm::vec3& i2, const::glm::vec3& i3);

        // TODO: Method to generate vertex data (including normals and uvs) and indices for sphere

        // TODO:
        //template <class VertexType>
        //static void addQuad(std::vector<VertexType>* vertices);

        // NOTE: Need a good way to generate a face in any orientation, at the very least Axis-Aligned
        static void addTopFace(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec2& halfSize = glm::vec3(0.5f),
                float* uvCoords = defaultUVMap
                );

        static void addQuad(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec2& halfSize = glm::vec3(0.5f));
        // Setup vertices and indices for 3D Axis-Aligned Bounding Box, by default 1f sized cube centered around the origin
        static void addAABB(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec3& halfSize = glm::vec3(0.5f));

        private:
        // Min X(S), Max X(S), MIN Y(T), MAX Y(T)
        static float defaultUVMap[4];
    };
}

#endif // __MESH_UTILS__
