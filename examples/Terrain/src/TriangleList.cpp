#include "TriangleList.hpp"

#include "Terrain.hpp"

void TriangleList::createTriangleList(int w, int d, const Terrain* terrain) {
    width = w;
    depth = d;

    createGLState();

    populateBuffers(terrain);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleList::render() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (depth - 1) * (width - 1) * 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void TriangleList::createGLState() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // TODO: Refactor to use VertexType structs to abstract these gl calls
    int POS_LOC = 0;
    int TEX_LOC = 1;
    size_t numFloats = 0;

    glEnableVertexAttribArray(POS_LOC);
    glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3;

    glEnableVertexAttribArray(TEX_LOC);
    glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3;
}

void TriangleList::populateBuffers(const Terrain* terrain) {
    std::vector<Vertex> vertices;
    vertices.resize(width * depth);
    initVertices(terrain, vertices);

    std::vector<unsigned int> indices;
    int numQuads = (width - 1) * (depth - 1);
    indices.resize(numQuads * 6);
    initIndices(indices);

    calcNormals(vertices, indices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void TriangleList::Vertex::initVertex(const Terrain* terrain, int x, int z) {
    float y = terrain->getHeight(x, z);
    Pos = glm::vec3(x * terrain->getWorldScale(), y, z * terrain->getWorldScale());

    float size = (float)terrain->getSize();
    UV = glm::vec2(terrain->getTextureScale() * x / size, terrain->getTextureScale() * z / size);
}

void TriangleList::initVertices(const Terrain* terrain, std::vector<Vertex>& vertices) {
    int index = 0;

    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            assert(index < vertices.size());
            vertices[index].initVertex(terrain, x, z);
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

void TriangleList::calcNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    unsigned index = 0;

    // Calculate each triangle's normal and accumulate it
    for (unsigned int i = 0; i < indices.size(); i+=3) {
        // NOTE: Pretty much identical to calculate normals method in MeshUtils class
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v1 = vertices[i1].Pos- vertices[i0].Pos;
        glm::vec3 v2 = vertices[i2].Pos- vertices[i0].Pos;
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        // Vertex normal is average of all face(triangle) normals sharing vertex
        vertices[i0].Normal += normal;
        vertices[i1].Normal += normal;
        vertices[i2].Normal += normal;
    }

    // normalize all vertex normals
    for (unsigned int i = 0; i < vertices.size(); i+=3) {
        vertices[i].Normal = glm::normalize(vertices[i].Normal);
    }
}
