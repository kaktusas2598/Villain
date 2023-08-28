#pragma once

#include "glm/glm.hpp"
#include "rendering/IndexBuffer.hpp"
#include "rendering/VertexArray.hpp"
#include "rendering/Vertex.hpp"

#include <memory>

namespace Villain {

    class Terrain;

    // Basic Terrain mesh builder without LOD
    class TriangleList {
        public:
            TriangleList() {}

            void createTriangleList(int w, int d, const Terrain* terrain);
            void render();

        private:
            VertexP1N1UV initVertex(const Terrain* terrain, int x, int z);

            int width, depth; //<<< Heights will be read from the height map

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            void populateBuffers(const Terrain* terrain);
            void initVertices(const Terrain* terrain, std::vector<VertexP1N1UV>& vertices);
            void initIndices(std::vector<unsigned int>& indices);
            void calcNormals(std::vector<VertexP1N1UV>& vertices, std::vector<unsigned int>& indices);
    };
}
