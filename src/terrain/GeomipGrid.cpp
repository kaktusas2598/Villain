#include "GeomipGrid.hpp"

#include "Terrain.hpp"
#include "rendering/MeshUtils.hpp"

// TODO: move somewhere else maybe
#define powi(base,exp) (int)powf((float)(base), (float)(exp))

namespace Villain {

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
        baseTerrain = terrain;

        numPatchesX = (width - 1) / (patchSize - 1);
        numPatchesZ = (depth - 1) / (patchSize - 1);
        maxLOD = lodManager.initLodManager(patchSize, numPatchesX, numPatchesZ, terrain->getWorldScale());
        lodInfo.resize(maxLOD + 1);

        populateBuffers(terrain);
    }

    void GeomipGrid::render(const glm::vec3& cameraPos, const glm::mat4& viewProj) {

        lodManager.update(cameraPos);

        FrustumCulling fc(viewProj);

        vao->bind();

        // Render every patch of terrain
        for (int patchZ = 0; patchZ < numPatchesZ; patchZ++) {
            for (int patchX = 0; patchX < numPatchesX; patchX++) {
                int z = patchZ * (patchSize - 1);
                int x = patchX * (patchSize - 1);
                // Frustum culling
                //if (!isPatchInsideViewFrustum_ClipSpace(x, z, viewProj)) {
                // Supposed to be more optimized than clip space culling because atm terrain coords are already in world space
                if (!isPatchInsideViewFrustum_WorldSpace(x, z, fc)) {
                    continue;
                }

                // Get LOD configuration for current patch
                const LODManager::PatchLOD& plod = lodManager.getPatchLOD(patchX, patchZ);
                int C = plod.Core;
                int L = plod.Left;
                int R = plod.Right;
                int T = plod.Top;
                int B = plod.Bottom;

                size_t baseIndex = sizeof(unsigned int) * lodInfo[C].Info[L][R][T][B].Start;
                int baseVertex = z * width + x;
                glDrawElementsBaseVertex(GL_TRIANGLES, lodInfo[C].Info[L][R][T][B].Count, GL_UNSIGNED_INT, (void*)baseIndex, baseVertex);
            }
        }
    }

    void GeomipGrid::populateBuffers(const Terrain* terrain) {
        std::vector<VertexP1N1UV> vertices;
        vertices.resize(width * depth);
        initVertices(terrain, vertices);

        int numIndices = calcNumIndices();
        std::vector<unsigned int> indices;
        indices.resize(numIndices);
        numIndices = initIndices(indices);
        printf("Final number of indices: %d\n", numIndices);

        calcNormals(vertices, indices);

        vao = std::make_unique<VertexArray>();
        vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(VertexP1N1UV));

        vao->addBuffer(*vbo, VertexP1N1UV::getVertexLayout());
        ibo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
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

    VertexP1N1UV GeomipGrid::initVertex(const Terrain* terrain, int x, int z) {
        VertexP1N1UV v;
        float y = terrain->getHeight(x, z);
        float worldScale = (float)terrain->getWorldScale();
        v.Position = glm::vec3(x * worldScale, y, z * worldScale);

        float size = (float)terrain->getSize();
        float textureScale = (float)terrain->getTextureScale();
        v.UV = glm::vec2(textureScale * x / size, textureScale * z / size);

        return v;
    }

    void GeomipGrid::initVertices(const Terrain* terrain, std::vector<VertexP1N1UV>& vertices) {
        int index = 0;

        for (int z = 0; z < depth; z++) {
            for (int x = 0; x < width; x++) {
                assert(index < vertices.size());
                vertices[index] = initVertex(terrain, x, z);
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

    void GeomipGrid::calcNormals(std::vector<VertexP1N1UV>& vertices, std::vector<unsigned int>& indices) {
        unsigned index = 0;

        // Calculate each triangle's normal and accumulate it
        for (int z = 0; z < depth - 1; z += (patchSize - 1)) {
            for (int x = 0; x < width - 1; x += (patchSize - 1)) {
                int baseVertex = z * width + x;
                int numIndices = lodInfo[0].Info[0][0][0][0].Count;
                for (unsigned int i = 0; i < numIndices; i += 3) {
                    unsigned int i0 = baseVertex + indices[i];
                    unsigned int i1 = baseVertex + indices[i + 1];
                    unsigned int i2 = baseVertex + indices[i + 2];

                    glm::vec3 normal = MeshUtils<VertexP1N1UV>::getNormal(vertices[i0].Position, vertices[i1].Position, vertices[i2].Position);

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

    bool GeomipGrid::isPatchInsideViewFrustum_ClipSpace(int x, int z, const glm::mat4& viewProj) {
        int x0 = x;
        int x1 = x + patchSize - 1;
        int z0 = z;
        int z1 = z + patchSize - 1;

        glm::vec3 p00(x0 * baseTerrain->getWorldScale(), baseTerrain->getHeight(x0, z0), z0 * baseTerrain->getWorldScale());
        glm::vec3 p01(x0 * baseTerrain->getWorldScale(), baseTerrain->getHeight(x0, z1), z1 * baseTerrain->getWorldScale());
        glm::vec3 p10(x1 * baseTerrain->getWorldScale(), baseTerrain->getHeight(x1, z0), z0 * baseTerrain->getWorldScale());
        glm::vec3 p11(x1 * baseTerrain->getWorldScale(), baseTerrain->getHeight(x1, z1), z1 * baseTerrain->getWorldScale());

        bool insideViewFrustum =
            isPointInsideViewFrustum(p00, viewProj) ||
            isPointInsideViewFrustum(p01, viewProj) ||
            isPointInsideViewFrustum(p10, viewProj) ||
            isPointInsideViewFrustum(p11, viewProj);

        return insideViewFrustum;
    }

    bool GeomipGrid::isPointInsideViewFrustum(const glm::vec3 point, const glm::mat4& viewProj) {
        glm::vec4 p4D(point, 1.0f);
        glm::vec4 clipSpaceP = viewProj * p4D;

        // Check if point (x, y, z) is inside clip space [-w, w]
        bool insideViewFrustum = ((clipSpaceP.x <= clipSpaceP.w) &&
                (clipSpaceP.x >= -clipSpaceP.w) &&
                (clipSpaceP.y <= clipSpaceP.w) &&
                (clipSpaceP.y >= -clipSpaceP.w) &&
                (clipSpaceP.z <= clipSpaceP.w) &&
                (clipSpaceP.z >= -clipSpaceP.w));

        return insideViewFrustum;
    }

    bool GeomipGrid::isPatchInsideViewFrustum_WorldSpace(int x, int z, FrustumCulling& fc) {
        int x0 = x;
        int x1 = x + patchSize - 1;
        int z0 = z;
        int z1 = z + patchSize - 1;

        float h00 = baseTerrain->getHeight(x0, z0);
        float h01 = baseTerrain->getHeight(x0, z1);
        float h10 = baseTerrain->getHeight(x1, z0);
        float h11 = baseTerrain->getHeight(x1, z1);

        float minHeight = std::min(h00, std::min(h01, std::min(h10, h11)));
        float maxHeight = std::max(h00, std::max(h01, std::max(h10, h11)));

        // Create points for bounding box
        glm::vec3 p00_low(x0 * baseTerrain->getWorldScale(), minHeight, z0 * baseTerrain->getWorldScale());
        glm::vec3 p01_low(x0 * baseTerrain->getWorldScale(), minHeight, z1 * baseTerrain->getWorldScale());
        glm::vec3 p10_low(x1 * baseTerrain->getWorldScale(), minHeight, z0 * baseTerrain->getWorldScale());
        glm::vec3 p11_low(x1 * baseTerrain->getWorldScale(), minHeight, z1 * baseTerrain->getWorldScale());

        glm::vec3 p00_high(x0 * baseTerrain->getWorldScale(), maxHeight, z0 * baseTerrain->getWorldScale());
        glm::vec3 p01_high(x0 * baseTerrain->getWorldScale(), maxHeight, z1 * baseTerrain->getWorldScale());
        glm::vec3 p10_high(x1 * baseTerrain->getWorldScale(), maxHeight, z0 * baseTerrain->getWorldScale());
        glm::vec3 p11_high(x1 * baseTerrain->getWorldScale(), maxHeight, z1 * baseTerrain->getWorldScale());

        bool insideViewFrustum =
            fc.isPointInsideViewFrustum(p00_low) ||
            fc.isPointInsideViewFrustum(p01_low) ||
            fc.isPointInsideViewFrustum(p10_low) ||
            fc.isPointInsideViewFrustum(p11_low) ||
            fc.isPointInsideViewFrustum(p00_high) ||
            fc.isPointInsideViewFrustum(p01_high) ||
            fc.isPointInsideViewFrustum(p10_high) ||
            fc.isPointInsideViewFrustum(p11_high);

        return insideViewFrustum;
    }
}
