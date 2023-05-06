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
    glDrawArrays(GL_POINTS, 0, width * depth);
    glBindVertexArray(0);
}

void TriangleList::createGLState() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int POS_LOC = 0;
    size_t numFloats = 0;

    glEnableVertexAttribArray(POS_LOC);
    glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3; // Preparing for future here, but probably all of this will be handled by engine anyway
}

void TriangleList::populateBuffers(const Terrain* terrain) {
    std::vector<Vertex> vertices;
    vertices.resize(width * depth);

    initVertices(terrain, vertices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
}

void TriangleList::Vertex::initVertex(const Terrain* terrain, int x, int z) {
    float y = terrain->getHeight(x, z);
    Pos = glm::vec3(x, y, z);
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
