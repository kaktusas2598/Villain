#include "GeomipGrid.hpp"

#include "Terrain.hpp"

void GeomipGrid::createGeomipGrid(int xSize, int ySize, int patch, const Terrain* terrain) {

    if ((xSize - 1) % (patch - 1) != 0) {
        int recommendedWidth = ((xSize - 1 + patch - 1) / (patch - 1)) * (patch - 1) + 1;
        printf("Width - 1 (%d) must be divisible by patch size - 1 (%d)\n", xSize, patch);
        printf("Recommended width: %d\n", recommendedWidth);
    }

    if ((ySize - 1) % (patch - 1) != 0) {
        int recommendedDepth = ((ySize - 1 + patch - 1) / (patch - 1)) * (patch - 1) + 1;
        printf("Depth - 1 (%d) must be divisible by patch size - 1 (%d)\n", ySize, patch);
        printf("Recommended depth: %d\n", recommendedDepth);
    }

    // Best vertex patch size for creating Level of Detail is:
    // 2^n + 1
    if (patch < 3)
        exitWithError("Minimum patch size is 3");
    if (patch % 2 == 0)
        exitWithError("Patch size must be an odd number");

    width = xSize;
    depth = ySize;
    patchSize = patch;

    numPatchesX = (width - 1) / (patchSize - 1);
    numPatchesZ = (depth - 1) / (patchSize - 1);
    //maxLOD = lodManager.initLodManager(patchSize, numPatchesX, numPatchesZ, terrain->getWorldScale());
    //lodInfo.resize(maxLOD + 1);

    createGLState();

    populateBuffers(terrain);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeomipGrid::render() {
    glBindVertexArray(vao);

    for (int z = 0; z < depth - 1; z += (patchSize - 1)) {
        for (int x = 0; x < width - 1; x += (patchSize - 1)) {
            int baseVertex = z * width + x;
            // patch rendering
            glDrawElementsBaseVertex(GL_TRIANGLES, (patchSize - 1) * (patchSize - 1) * 6, GL_UNSIGNED_INT, NULL, baseVertex);
        }
    }

    glBindVertexArray(0);
}

void GeomipGrid::createGLState() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // TODO: Refactor to use VertexType structs to abstract these gl calls
    int POS_LOC = 0;
    int TEX_LOC = 1;
    int NORMAL_LOC = 2;
    size_t numFloats = 0;

    glEnableVertexAttribArray(POS_LOC);
    glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3;

    glEnableVertexAttribArray(TEX_LOC);
    glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3;

    glEnableVertexAttribArray(NORMAL_LOC);
    glVertexAttribPointer(NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(numFloats * sizeof(float)));
    numFloats += 3;
}

void GeomipGrid::populateBuffers(const Terrain* terrain) {
    std::vector<Vertex> vertices;
    vertices.resize(width * depth);
    initVertices(terrain, vertices);

    std::vector<unsigned int> indices;
    int numQuads = (patchSize - 1) * (patchSize - 1);
    indices.resize(numQuads * 6);
    initIndices(indices);

    calcNormals(vertices, indices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void GeomipGrid::Vertex::initVertex(const Terrain* terrain, int x, int z) {
    float y = terrain->getHeight(x, z);
    Pos = glm::vec3(x * terrain->getWorldScale(), y, z * terrain->getWorldScale());

    float size = (float)terrain->getSize();
    UV = glm::vec2(terrain->getTextureScale() * x / size, terrain->getTextureScale() * z / size);
}

void GeomipGrid::initVertices(const Terrain* terrain, std::vector<Vertex>& vertices) {
    int index = 0;

    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            assert(index < vertices.size());
            vertices[index].initVertex(terrain, x, z);
            index++;
        }
    }
}

void GeomipGrid::initIndices(std::vector<unsigned int>& indices) {
    int index = 0;

    for (int z = 0; z < patchSize - 1; z += 2) {
        for (int x = 0; x < patchSize - 1; x += 2) {
            unsigned int indexCenter = (z + 1) * width + x + 1;
            unsigned int indexTemp1 = z * width + x;
            unsigned int indexTemp2 = (z + 1) * width + x;

            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2 += width;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2++;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2++;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2 -= width;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2 -= width;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2--;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

            indexTemp1 = indexTemp2;
            indexTemp2--;
            index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

        }
    }

    assert(index == indices.size());
}

void GeomipGrid::calcNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    unsigned index = 0;

    // Calculate each triangle's normal and accumulate it
    for (int z = 0; z < depth - 1; z += (patchSize - 1)) {
        for (int x = 0; x < width - 1; x += (patchSize - 1)) {
            int baseVertex = z * width + x;
            for (unsigned int i = 0; i < indices.size(); i += 3) {
                unsigned int i0 = baseVertex + indices[i];
                unsigned int i1 = baseVertex + indices[i + 1];
                unsigned int i2 = baseVertex + indices[i + 2];

                glm::vec3 v1 = vertices[i1].Pos- vertices[i0].Pos;
                glm::vec3 v2 = vertices[i2].Pos- vertices[i0].Pos;
                glm::vec3 normal = glm::cross(v1, v2);
                normal = glm::normalize(normal);

                // Vertex normal is average of all face(triangle) normals sharing vertex
                vertices[i0].Normal += normal;
                vertices[i1].Normal += normal;
                vertices[i2].Normal += normal;
            }
        }
    }

    // normalize all vertex normals
    for (unsigned int i = 0; i < vertices.size(); i++) {
        vertices[i].Normal = glm::normalize(vertices[i].Normal);
    }
}


unsigned int GeomipGrid::addTriangle(unsigned int index, std::vector<unsigned int>& indices, unsigned v1, unsigned v2, unsigned v3) {
    assert(index < indices.size());
    indices[index++] = v1;
    assert(index < indices.size());
    indices[index++] = v2;
    assert(index < indices.size());
    indices[index++] = v3;

    return index;
}
