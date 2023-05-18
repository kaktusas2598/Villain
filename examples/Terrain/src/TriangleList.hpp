#ifndef __TRIANGLE_LIST__
#define __TRIANGLE_LIST__

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <vector>

class Terrain;

// GL Renderer for Terrain
// TODO: will need to refactor to fit in the engine
class TriangleList {
    public:
        TriangleList() {}

        void createTriangleList(int w, int d, const Terrain* terrain);
        void render();

    private:
        // TODO: refactor to use struct from Vertex.hpp instead
        struct Vertex {
            glm::vec3 Pos;
            glm::vec2 UV;
            glm::vec3 Normal{0.0f};

            void initVertex(const Terrain* terrain, int x, int z);
        };

        int width, depth; //<<< Heights will be read from the height map

        GLuint vao, vbo, ibo;

        void createGLState();
        void populateBuffers(const Terrain* terrain);
        void initVertices(const Terrain* terrain, std::vector<Vertex>& vertices);
        void initIndices(std::vector<unsigned int>& indices);
        void calcNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};

#endif // __TRIANGLE_LIST__
