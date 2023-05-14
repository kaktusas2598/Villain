#ifndef __TERRAIN__
#define __TERRAIN__

#include <string>

#include "Camera.hpp"
#include "TriangleList.hpp"
#include "rendering/Shader.hpp"

// NOTE: should this be called BaseTerrain to be able to have different types of terrain?
class Terrain {
    public:
        Terrain() {}

        void init(float scale = 1.0f);
        void render(Villain::Camera* camera);
        void loadFromFile(const std::string& fileName);
        void setTerrainTexture(Villain::Texture* tex) { textures[0] = tex; }

        float getHeight(int x, int z) const {
            // TODO: this needs to be much better, need out of bounds check and also
            // this assumes terrain will be square, will it always though?
            return heightMap[x * terrainSize + z];
        }
        float getSize() const { return terrainSize; }
        float getWorldScale() const { return worldScale; }

    protected:
        void loadHeightMap(const std::string& fileName);

        float* heightMap = nullptr;
        int terrainSize = 0;
        float worldScale = 1.0f;

        TriangleList triangleList;
        Villain::Shader* terrainShader;
        Villain::Texture* textures[4] = {nullptr};

        // Required for terrain generation
        float minHeight = 0.0f;
        float maxHeight = 0.0f;
};

#endif // __TERRAIN__
