#include "TriangleList.hpp"

#include "Terrain.hpp"

namespace Villain {

    void TriangleList::createTriangleList(int w, int d, const Terrain* terrain) {
        width = w;
        depth = d;

        populateBuffers(terrain);
    }

    void TriangleList::render() {
        vao->bind();
        glDrawElements(GL_TRIANGLES, (depth - 1) * (width - 1) * 6, GL_UNSIGNED_INT, NULL);
    }

    void TriangleList::populateBuffers(const Terrain* terrain) {
        std::vector<VertexP1N1UV> vertices;
        vertices.resize(width * depth);
        initVertices(terrain, vertices);

        std::vector<unsigned int> indices;
        int numQuads = (width - 1) * (depth - 1);
        indices.resize(numQuads * 6);
        initIndices(indices);

        calcNormals(vertices, indices);

        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(VertexP1N1UV));

        vao->addBuffer(*vbo, VertexP1N1UV::getVertexLayout());
        ibo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
    }

    VertexP1N1UV TriangleList::initVertex(const Terrain* terrain, int x, int z) {
        VertexP1N1UV v;
        float y = terrain->getHeight(x, z);
        v.Position = glm::vec3(x * terrain->getWorldScale(), y, z * terrain->getWorldScale());

        float size = (float)terrain->getSize();
        v.UV = glm::vec2(terrain->getTextureScale() * x / size, terrain->getTextureScale() * z / size);
        return v;
    }

    void TriangleList::initVertices(const Terrain* terrain, std::vector<VertexP1N1UV>& vertices) {
        int index = 0;

        for (int z = 0; z < depth; z++) {
            for (int x = 0; x < width; x++) {
                assert(index < vertices.size());
                vertices[index] = initVertex(terrain, x, z);
                index++;
            }
        }
    }

    void TriangleList::initIndices(std::vector<unsigned int>& indices) {
        int index = 0;

        for (int z = 0; z < depth - 1; z++) {
            for (int x = 0; x < width - 1; x++) {
                unsigned int indexBottomLeft = z * width + x;
                unsigned int indexTopLeft = (z + 1) * width + x;
                unsigned int indexTopRight = (z + 1) * width + x + 1;
                unsigned int indexBottomRight = z * width + x + 1;

                // Top left triangle
                indices[index++] = indexBottomLeft;
                indices[index++] = indexTopLeft;
                indices[index++] = indexTopRight;

                // Bottom right triangle
                indices[index++] = indexBottomLeft;
                indices[index++] = indexTopRight;
                indices[index++] = indexBottomRight;
            }
        }
    }

    void TriangleList::calcNormals(std::vector<VertexP1N1UV>& vertices, std::vector<unsigned int>& indices) {
        unsigned index = 0;

        // Calculate each triangle's normal and accumulate it
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            // NOTE: Pretty much identical to calculate normals method in MeshUtils class
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            glm::vec3 v1 = vertices[i1].Position - vertices[i0].Position;
            glm::vec3 v2 = vertices[i2].Position - vertices[i0].Position;
            glm::vec3 normal = glm::cross(v1, v2);
            normal = glm::normalize(normal);

            // Vertex normal is average of all face(triangle) normals sharing vertex
            vertices[i0].Normal += normal;
            vertices[i1].Normal += normal;
            vertices[i2].Normal += normal;
        }

        // normalize all vertex normals
        for (unsigned int i = 0; i < vertices.size(); i++) {
            vertices[i].Normal = glm::normalize(vertices[i].Normal);
        }
    }
}
