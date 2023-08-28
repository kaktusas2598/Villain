#pragma once

#include "glm/glm.hpp"
#include "rendering/Vertex.hpp"
#include <GL/glew.h>
#include <vector>

namespace Villain {

    class Terrain;

    // GL Renderer for Terrain
    // TODO: will need to refactor to fit in the engine
    class TriangleList {
        public:
            TriangleList() {}

            void createTriangleList(int w, int d, const Terrain* terrain);
            void render();

        private:
            VertexP1N1UV initVertex(const Terrain* terrain, int x, int z);

            int width, depth; //<<< Heights will be read from the height map

            GLuint vao, vbo, ibo;

            void createGLState();
            void populateBuffers(const Terrain* terrain);
            void initVertices(const Terrain* terrain, std::vector<VertexP1N1UV>& vertices);
            void initIndices(std::vector<unsigned int>& indices);
            void calcNormals(std::vector<VertexP1N1UV>& vertices, std::vector<unsigned int>& indices);
    };
}
