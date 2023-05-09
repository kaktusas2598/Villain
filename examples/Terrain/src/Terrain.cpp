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

void Terrain::render(Villain::Camera* camera) {
    terrainShader->bind();
    terrainShader->setUniform1f("minHeight", minHeight);
    terrainShader->setUniform1f("maxHeight", maxHeight);
    terrainShader->setUniformMat4f("projection", camera->getProjMatrix());
    terrainShader->setUniformMat4f("view", camera->getViewMatrix());
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
