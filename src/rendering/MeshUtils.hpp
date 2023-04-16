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

        // TODO:
        template <class VertexType>
        static void addQuad(std::vector<VertexType>* vertices);


        // TEMP method for now to get some floors
        static void addTopFace(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices);
        // Setup vertices and indices for cube
        static void addCube(
                std::vector<VertexP1N1UV>* vertices,
                std::vector<unsigned int>* indices,
                const glm::vec3& center = glm::vec3(0.0f),
                const glm::vec3& size = glm::vec3(1.0f));
    };
}

#endif // __MESH_UTILS__
