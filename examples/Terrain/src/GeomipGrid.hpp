#ifndef __GEO_MIPMAPPING_GRID__
#define __GEO_MIPMAPPING_GRID__

#include "LODManager.hpp"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <vector>

class Terrain;

// GL Renderer for Terrain
// TODO: will need to refactor to fit in the engine
class GeomipGrid {
    public:
        GeomipGrid() {}

        void createGeomipGrid(int xSize, int ySize, int patch, const Terrain* terrain);
        void render();
        // TODO: implement
        void destroy();

    private:
        // TODO: refactor to use struct from Vertex.hpp instead
        struct Vertex {
            glm::vec3 Pos;
            glm::vec2 UV;
            glm::vec3 Normal{0.0f};

            void initVertex(const Terrain* terrain, int x, int z);
        };

        int width, depth; //<<< Heights will be read from the height map
        int patchSize = 0;
        int maxLOD = 0;

        GLuint vao, vbo, ibo;

        struct SingleLODInfo {
            int Start = 0;
            int Count = 0;
        };

#define LOD_LEFT   2
#define LOD_RIGHT  2
#define LOD_TOP    2
#define LOD_BOTTOM 2

        struct LODInfo {
            SingleLODInfo Info[LOD_LEFT][LOD_RIGHT][LOD_TOP][LOD_BOTTOM];
        };

        std::vector<LODInfo> lodInfo;
        int numPatchesX = 0;
        int numPatchesZ = 0;
        LODManager lodManager;

        void createGLState();
        void populateBuffers(const Terrain* terrain);
        void initVertices(const Terrain* terrain, std::vector<Vertex>& vertices);
        void initIndices(std::vector<unsigned int>& indices);
        void calcNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        unsigned int addTriangle(unsigned int index, std::vector<unsigned int>& indices, unsigned v1, unsigned v2, unsigned v3);
};


#endif // __GEO_MIPMAPPING_GRID__
