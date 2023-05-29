#include "GeomipGrid.hpp"

#include "Terrain.hpp"

// TODO: move somewhere else maybe
#define powi(base,exp) (int)powf((float)(base), (float)(exp))

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
    maxLOD = lodManager.initLodManager(patchSize, numPatchesX, numPatchesZ, terrain->getWorldScale());
    lodInfo.resize(maxLOD + 1);

    createGLState();

    populateBuffers(terrain);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeomipGrid::render(const glm::vec3& cameraPos) {

    lodManager.update(cameraPos);
    glBindVertexArray(vao);

    // Render every patch of terrain
    for (int patchZ = 0; patchZ < numPatchesZ; patchZ++) {
        for (int patchX = 0; patchX < numPatchesX; patchX++) {
            // Get LOD configuration for current patch
            const LODManager::PatchLOD& plod = lodManager.getPatchLOD(patchX, patchZ);
            int C = plod.Core;
            int L = plod.Left;
            int R = plod.Right;
            int T = plod.Top;
            int B = plod.Bottom;

            size_t baseIndex = sizeof(unsigned int) * lodInfo[C].Info[L][R][T][B].Start;

            int z = patchZ * (patchSize - 1);
            int x = patchX * (patchSize - 1);
            int baseVertex = z * width + x;
            glDrawElementsBaseVertex(GL_TRIANGLES, lodInfo[C].Info[L][R][T][B].Count, GL_UNSIGNED_INT, (void*)baseIndex, baseVertex);
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

    int numIndices = calcNumIndices();
    std::vector<unsigned int> indices;
    indices.resize(numIndices);
    numIndices = initIndices(indices);
    printf("Final number of indices: %d\n", numIndices);

    calcNormals(vertices, indices);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

int GeomipGrid::calcNumIndices() {
    int numQuads = (patchSize - 1) * (patchSize - 1);
    int numIndices = 0;
    int maxPermutationsPerLevel = 16;
    const int indicesPerQuad = 6;
    for (int lod = 0; lod <= maxLOD; lod++) {
        printf("LOD: %d Num Quads: %d\n", lod, numQuads);
        numIndices += numQuads * indicesPerQuad * maxPermutationsPerLevel;
        numQuads /= 4;
    }
    printf("Initial number of indices: %d\n", numIndices);

    return numIndices;
}

void GeomipGrid::Vertex::initVertex(const Terrain* terrain, int x, int z) {
    float y = terrain->getHeight(x, z);
    float worldScale = (float)terrain->getWorldScale();
    Pos = glm::vec3(x * worldScale, y, z * worldScale);

    float size = (float)terrain->getSize();
    float textureScale = (float)terrain->getTextureScale();
    UV = glm::vec2(textureScale * x / size, textureScale * z / size);
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

int GeomipGrid::initIndices(std::vector<unsigned int>& indices) {
    int index = 0;

    for (int lod = 0; lod <= maxLOD; lod++) {
        index = initIndicesLOD(index, indices, lod);
    }

    return index;
}

int GeomipGrid::initIndicesLOD(int index, std::vector<unsigned int>& indices, int lod) {
    int totalIndicesForLOD = 0;

    for (int l = 0; l < LOD_LEFT; l++) {
        for (int r = 0; r < LOD_RIGHT; r++) {
            for (int t = 0; t < LOD_TOP; t++) {
                for (int b = 0; b < LOD_BOTTOM; b++) {
                    lodInfo[lod].Info[l][r][t][b].Start = index;
                    index = initIndicesLODSingle(index, indices, lod, lod + l, lod + r, lod + t, lod + b);

                    lodInfo[lod].Info[l][r][t][b].Count = index - lodInfo[lod].Info[l][r][t][b].Start;
                    totalIndicesForLOD += lodInfo[lod].Info[l][r][t][b].Count;
                }
            }
        }
    }

    return index;
}

int GeomipGrid::initIndicesLODSingle(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom) {
    int fanStep = powi(2, lodCore + 1); // lod 0 --> 2, lod 1 --> 4, lod 2 --> 8, etc..
    int endPos = patchSize - 1 - fanStep;

    for (int z = 0; z <= endPos; z += fanStep) {
        for (int x = 0; x <= endPos; x += fanStep) {
            int lLeft   = x == 0      ? lodLeft : lodCore;
            int lRight  = x == endPos ? lodRight : lodCore;
            int lTop    = z == endPos ? lodTop : lodCore;
            int lBottom = z == 0      ? lodBottom : lodCore;

            index = createTriangleFan(index, indices, lodCore, lLeft, lRight, lTop, lBottom, x, z);
        }
    }

    return index;
}

unsigned int GeomipGrid::createTriangleFan(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z) {
    int stepLeft = powi(2, lodLeft);
    int stepRight = powi(2, lodRight);
    int stepTop = powi(2, lodTop);
    int stepBottom = powi(2, lodBottom);
    int stepCenter = powi(2, lodCore);

    uint indexCenter = (z + stepCenter) * width + x + stepCenter;

    // first up
    uint indexTemp1 = z * width + x;
    uint indexTemp2 = (z + stepLeft) * width + x;
    index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    // second up
    if (lodLeft == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 += stepLeft * width;
        index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    // first right
    indexTemp1 = indexTemp2;
    indexTemp2 += stepTop;
    index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    // second right
    if (lodTop == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 += stepTop;
        index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    // first down
    indexTemp1 = indexTemp2;
    indexTemp2 -= stepRight * width;
    index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    // second down
    if (lodRight == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 -= stepRight * width;
        index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    // first left
    indexTemp1 = indexTemp2;
    indexTemp2 -= stepBottom;
    index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);

    // second left
    if (lodBottom == lodCore) {
        indexTemp1 = indexTemp2;
        indexTemp2 -= stepBottom;
        index = addTriangle(index, indices, indexCenter, indexTemp1, indexTemp2);
    }

    return index;
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
