#include "Terrain.hpp"

#include "FileUtils.hpp"
#include <cassert>
#include <cmath>


void Terrain::loadFromFile(const std::string& fileName) {
    loadHeightMap(fileName);

    triangleList.createTriangleList(terrainSize, terrainSize, this);
}

void Terrain::init(float scale) {
    worldScale = scale;
    // TODO: will need to move it somewhere else, terrain shader will probably be provided by the engine
    terrainShader = new Villain::Shader("assets/shaders/terrain.glsl");
}

void Terrain::destroy() {
    if (heightMap) {
        delete heightMap;
        heightMap = nullptr;
    }
}

void Terrain::render(Villain::Camera* camera) {
    terrainShader->bind();
    terrainShader->setUniform1f("minHeight", minHeight);
    terrainShader->setUniform1f("maxHeight", maxHeight);
    terrainShader->setUniformMat4f("projection", camera->getProjMatrix());
    terrainShader->setUniformMat4f("view", camera->getViewMatrix());
    // NOTE: 4 is max supported terrain textures ATM
    textures[0]->bind();
    //for (int i = 0; i < 4; i++) {
        //if (textures[i] != nullptr) {
            //textures[i]->bind(i);
        //}
    //}
    triangleList.render();
}

// TODO: besides binary file full of floats, basic terrain should also support image height maps
void Terrain::loadHeightMap(const std::string& fileName) {
    int fileSize;
    heightMap = (float*)Villain::FileUtils::loadBinaryFile(fileName, fileSize);

    // Make sure file contains whole number of floats
    assert(fileSize % sizeof(float) == 0);

    terrainSize = sqrtf((float)fileSize / sizeof(float));

    // Find min/maxHeight
    for (int i = 0; i < terrainSize * terrainSize; i++) {
        if (heightMap[i] > maxHeight)
            maxHeight = heightMap[i];
        if (heightMap[i] < minHeight)
            minHeight = heightMap[i];
    }
}

float Terrain::getHeightInterpolated(float x, float z) const {
    float baseHeight = getHeight((int)x, (int)z);

    if (((int)x + 1 >= terrainSize) || ((int)z + 1 >= terrainSize)) {
        return baseHeight;
    }

    float nextXHeight = getHeight(x + 1, z);
    float ratioX = x - floorf(x);
    float interpolatedHeightX = (float)(nextXHeight - baseHeight) * ratioX + (float)baseHeight;

    float nextZHeight = getHeight(x, z + 1);
    float ratioZ = z - floorf(z);
    float interpolatedHeightZ = (float)(nextZHeight - baseHeight) * ratioZ + (float)baseHeight;

    float finalHeight = (interpolatedHeightX + interpolatedHeightZ) / 2.0f;
    return finalHeight;
}
