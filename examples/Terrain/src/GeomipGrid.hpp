#ifndef __GEO_MIPMAPPING_GRID__
#define __GEO_MIPMAPPING_GRID__

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

        GLuint vao, vbo, ibo;

        void createGLState();
        void populateBuffers(const Terrain* terrain);
        void initVertices(const Terrain* terrain, std::vector<Vertex>& vertices);
        void initIndices(std::vector<unsigned int>& indices);
        void calcNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        unsigned int addTriangle(unsigned int index, std::vector<unsigned int>& indices, unsigned v1, unsigned v2, unsigned v3);
};


#endif // __GEO_MIPMAPPING_GRID__
