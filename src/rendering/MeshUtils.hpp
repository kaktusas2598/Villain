#ifndef __MESH_UTILS__
#define __MESH_UTILS__

#include <vector>

namespace Villain {

    // Various utility methods to generate vertex data, primitives
    class MeshUtils {
        public:

        /*
         * Generate indices for a quad
         *
         * @param int startLocation - starting vertex location/index
         * @param bool direction - winding order
         */
        static void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);
    };
}

#endif // __MESH_UTILS__
