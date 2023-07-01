#include "Terrain.hpp"

#include "FileUtils.hpp"
#include "ResourceManager.hpp"
#include <cassert>
#include <cmath>


void Terrain::loadFromFile(const std::string& fileName, int patch) {
    loadHeightMap(fileName);
    patchSize = patch;

    if (patch == 0) {
        triangleList.createTriangleList(terrainSize, terrainSize, this);
    } else {
        geomipGrid.createGeomipGrid(terrainSize, terrainSize, patchSize, this);
        useLOD = true;
    }
}

void Terrain::init(float scale, float texScale, std::vector<std::string> textureFilenames) {
    worldScale = scale;
    textureScale = texScale;

    for (int i = 0; i < textureFilenames.size(); i++) {
        textures[i] = Villain::ResourceManager::Instance()->loadTexture(textureFilenames[i], textureFilenames[i], GL_REPEAT);
    }
    // TODO: will need to move it somewhere else, terrain shader will probably be provided by the engine
    //terrainShader = new Villain::Shader("assets/shaders/terrain.glsl");
    if (textureFilenames.size())
        terrainShader = new Villain::Shader("assets/shaders/multiTexTerrain.glsl");
    else
        terrainShader = Villain::Shader::createFromResource("basicTerrain");
}

void Terrain::destroy() {
    if (heightMap) {
        delete heightMap;
        heightMap = nullptr;
    }
}

void Terrain::render(Villain::RenderingEngine& renderingEngine, Villain::Camera* camera) {
    terrainShader->bind();
    terrainShader->setUniform1f("minHeight", minHeight);
    terrainShader->setUniform1f("maxHeight", maxHeight);
    terrainShader->setUniformMat4f("projection", camera->getProjMatrix());
    terrainShader->setUniformMat4f("view", camera->getViewMatrix());

    // Specific to multi texture shader
    terrainShader->setUniform1i("textureHeight0", 0);
    terrainShader->setUniform1i("textureHeight1", 1);
    terrainShader->setUniform1i("textureHeight2", 2);
    terrainShader->setUniform1i("textureHeight3", 3);
    terrainShader->setUniform1f("height0", height0);
    terrainShader->setUniform1f("height1", height1);
    terrainShader->setUniform1f("height2", height2);
    terrainShader->setUniform1f("height3", height3);

    terrainShader->setUniformVec3("reverseLightDir", lightDirection);

    for (int i = 0; i < 4; i++) {
        if (textures[i]) {
            textures[i]->bind(i);
            terrainShader->setUniform1i("useTexture", 1);
        }
    }

    terrainShader->setFogUniforms(renderingEngine, *camera);

    if (useLOD) {
        geomipGrid.render(camera->getPosition(), camera->getProjMatrix() * camera->getViewMatrix());
    } else {
        triangleList.render();
    }
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

glm::vec3 Terrain::constrainPositionRelativeToTerrain(const glm::vec3& pos) {
    glm::vec3 newPos = pos;

    // Make sure new position does not go out of bounds of terrain
    if (pos.x < 0.0f)
        newPos.x = 0.0f;

    if (pos.z < 0.0f)
        newPos.z = 0.0f;

    if (pos.x > getWorldSize())
        newPos.x = getWorldSize() - 0.5f;

    if (pos.z > getWorldSize())
        newPos.z = getWorldSize() - 0.5f;

    // TODO: need to pass in probably, this could be players, vehicle height as well
    const float bodyHeight = 50.0f;
    newPos.y = getWorldHeight(pos.x, pos.z) + bodyHeight;

    // Optional: simulate walking (slighly shifting up and down)
    float f = sinf(pos.x * 4.0f) + cosf(pos.z * 4.0f);
    f /= 35.0f;
    newPos.y += f;

    return newPos;
}

float Terrain::getWorldHeight(float x, float z) {
    // Original height map is not scaled, so we need to scale coordinates back down before getting world height
    float heightMapX = x / worldScale;
    float heightMapZ = z / worldScale;

    return getHeightInterpolatedForCollision(heightMapX, heightMapZ);
}

float Terrain::getHeightInterpolatedForCollision(float x, float z) const {
    float x0z0Height = getHeight(x, z);

    if(((int)x + 1 >= terrainSize) || ((int)z + 1) >= terrainSize) {
        return x0z0Height;
    }

    float x1z0Height = getHeight((int)x + 1, z);
    float x0z1Height = getHeight(x, (int)z + 1);
    float x1z1Height = getHeight((int)x + 1, (int)z + 1);

    // Calculate height for collision detection using bilinear interpolation
    // NOTE: Alternative method for getting height might be using biraycentric coordinates
    float factorX = x - floorf(x);
    float interpolatedBottom = (x1z0Height - x0z0Height) * factorX + x0z0Height;
    float interpolatedTop = (x1z1Height - x0z1Height) * factorX + x0z1Height;

    float factorZ = z - floorf(z);
    float finalHeight = (interpolatedTop - interpolatedBottom) * factorZ + interpolatedBottom;

    return finalHeight;
}
