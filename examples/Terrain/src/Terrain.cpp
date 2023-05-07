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
    terrainShader->setUniformMat4f("projection", camera->getProjMatrix());
    terrainShader->setUniformMat4f("view", camera->getViewMatrix());
    triangleList.render();
}

void Terrain::loadHeightMap(const std::string& fileName) {
    // TODO: load binary file here, height map texture will have to convert colors to floats
    int fileSize;
    heightMap = (float*)Villain::FileUtils::loadBinaryFile(fileName, fileSize);

    // Make sure file contains whole number of floats
    assert(fileSize % sizeof(float) == 0);

    terrainSize = sqrtf(fileSize / sizeof(float));
}
