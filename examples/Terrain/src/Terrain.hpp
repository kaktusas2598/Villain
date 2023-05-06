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

        void init();
        void render(Villain::Camera* camera);
        void loadFromFile(const std::string& fileName);

        float getHeight(int x, int z) const {
            // TODO: this needs to be much better, need out of bounds check and also
            // this assumes terrain will be square, will it always though?
            return heightMap[x * terrainSize + z];
        }
    protected:
        void loadHeightMap(const std::string& fileName);

        float* heightMap = nullptr;
        int terrainSize = 0;

        TriangleList triangleList;
        Villain::Shader* terrainShader;
};

#endif // __TERRAIN__
