#ifndef __TERRAIN__
#define __TERRAIN__

#include <string>

#include "Camera.hpp"
#include "GeomipGrid.hpp"
#include "TriangleList.hpp"
#include "rendering/Shader.hpp"

// NOTE: should this be called BaseTerrain to be able to have different types of terrain?
class Terrain {
    public:
        Terrain() {}

        void init(float scale = 1.0f, float texScale = 1.0f, std::vector<std::string> textureFilenames={});
        void render(Villain::Camera* camera);
        void loadFromFile(const std::string& fileName);
        void setTerrainTexture(Villain::Texture* tex) { textures[0] = tex; }
        void setLightDirection(glm::vec3& lightDir) { lightDirection = lightDir; }
        void destroy();

        float* getHeightMap() { return heightMap; }
        float getHeight(int x, int z) const {
            // TODO: this needs to be much better, need out of bounds check and also
            // this assumes terrain will be square, will it always though?
            return heightMap[x * terrainSize + z];
        }
        float getHeightInterpolated(float x, float z) const;
        float getSize() const { return terrainSize; }
        float getWorldScale() const { return worldScale; }
        float getTextureScale() const { return textureScale; }
        float getMinHeight() const { return minHeight; }
        float getMaxHeight() const { return maxHeight; }

        float* getHeight0() { return &height0; }
        float* getHeight1() { return &height1; }
        float* getHeight2() { return &height2; }
        float* getHeight3() { return &height3; }

    protected:
        void loadHeightMap(const std::string& fileName);

        float* heightMap = nullptr;
        int terrainSize = 0;
        int patchSize = 0;
        float worldScale = 1.0f;
        float textureScale = 1.0f;
        glm::vec3 lightDirection{0.0f};

        GeomipGrid geomipGrid;
        TriangleList triangleList;
        Villain::Shader* terrainShader;
        Villain::Texture* textures[4] = {nullptr};
        bool useLOD = false; //<< By default use simple terrain mesh without Level of Detail

        // Required for terrain generation
        float minHeight = 0.0f;
        float maxHeight = 0.0f;

        // Required when using multi texture terrain shader
        float height0 = 50.0f;
        float height1 = 100.0f;
        float height2 = 150.0f;
        float height3 = 250.0f;
};

#endif // __TERRAIN__
