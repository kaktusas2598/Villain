#pragma once

#include "glm/glm.hpp"
#include "rendering/IndexBuffer.hpp"
#include "rendering/Vertex.hpp"
#include "rendering/VertexArray.hpp"
#include "LODManager.hpp"
#include "TerrainFrustumCulling.hpp"

#include <memory>

namespace Villain {

    class Terrain;

    // Geomipmapped terrain mesh with Level of Detail(LOD) rendering
    class GeomipGrid {
        public:
            GeomipGrid() {}

            void createGeomipGrid(int xSize, int ySize, int patch, const Terrain* terrain);
            void render(const glm::vec3& cameraPos, const glm::mat4& viewProj);
            // TODO: implement
            void destroy();

        private:
            VertexP1N1UV initVertex(const Terrain* terrain, int x, int z);

            const Terrain* baseTerrain = nullptr;
            int width, depth; //<<< Heights will be read from the height map
            int patchSize = 0;
            int maxLOD = 0;

            std::unique_ptr<VertexArray> vao;
            std::unique_ptr<VertexBuffer> vbo;
            std::unique_ptr<IndexBuffer> ibo;

            struct SingleLODInfo {
                int Start = 0;
                int Count = 0;
            };

#define LOD_LEFT   2
#define LOD_RIGHT  2
#define LOD_TOP    2
#define LOD_BOTTOM 2

            struct LODInfo {
                SingleLODInfo Info[LOD_LEFT][LOD_RIGHT][LOD_TOP][LOD_BOTTOM];
            };

            std::vector<LODInfo> lodInfo;
            int numPatchesX = 0;
            int numPatchesZ = 0;
            LODManager lodManager;

            void populateBuffers(const Terrain* terrain);
            void initVertices(const Terrain* terrain, std::vector<VertexP1N1UV>& vertices);

            int initIndices(std::vector<unsigned int>& indices);
            int initIndicesLOD(int index, std::vector<unsigned int>& indices, int lod);
            int initIndicesLODSingle(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom);

            void calcNormals(std::vector<VertexP1N1UV>& vertices, std::vector<unsigned int>& indices);

            unsigned int addTriangle(unsigned int index, std::vector<unsigned int>& indices, unsigned v1, unsigned v2, unsigned v3);
            unsigned int createTriangleFan(int index, std::vector<unsigned int>& indices, int lodCore, int lodLeft, int lodRight, int lodTop, int lodBottom, int x, int z);
            int calcNumIndices();

            bool isPatchInsideViewFrustum_ClipSpace(int x, int z, const glm::mat4& viewProj);
            bool isPointInsideViewFrustum(const glm::vec3 point, const glm::mat4& viewProj);

            bool isPatchInsideViewFrustum_WorldSpace(int x, int z, FrustumCulling& fc);
    };
}
