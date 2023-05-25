#include "Terrain.hpp"

#include "FileUtils.hpp"
#include <cassert>
#include <cmath>


void Terrain::loadFromFile(const std::string& fileName) {
    loadHeightMap(fileName);

    triangleList.createTriangleList(terrainSize, terrainSize, this);
}

void Terrain::init(float scale, float texScale, std::vector<std::string> textureFilenames) {
    worldScale = scale;
    textureScale = texScale;

    for (int i = 0; i < textureFilenames.size(); i++) {
        textures[i] = new Villain::Texture(textureFilenames[i], GL_REPEAT);
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
