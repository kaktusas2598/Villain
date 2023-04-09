#include "Level.hpp"

#include "ResourceManager.hpp"
#include <iostream>

const float ROOM_WIDTH = 1.0f;
const float ROOM_LENGTH = 1.0f;
const float ROOM_HEIGHT = 2.0f;
const int NUM_TEX_EXP = 4;
const int NUM_TEXTURES = (int)pow(2, NUM_TEX_EXP);

Level::Level(const std::string& fileName, const std::string& tileAtlasFileName) {
    bitmap = new Bitmap(fileName);
    generateLevel(tileAtlasFileName);
}

void Level::generateLevel(const std::string& tileAtlasFileName) {
    // Load and generate 3D map from bitmap texture
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < bitmap->getWidth(); i++) {
        for (int j = 0; j < bitmap->getHeight(); j++) {
            Pixel pixel = bitmap->getPixel(i, j);
            if (pixel.R != 0 || pixel.G != 0 || pixel.B != 0) {
                float* texCoords = getTexCoords(pixel.G);
                // Floor vertices, normals are defaults just to make shader work
                addFace(&indices, vertices.size(), true);
                addVertices(&vertices, i, j, true, false, true, 0.0f, texCoords);

                // Ceilings, also revert indices to make sure textures are drawn from the bottom
                addFace(&indices, vertices.size(), false);
                addVertices(&vertices, i, j, true, false, true, 1.0f, texCoords);

                texCoords = getTexCoords(pixel.R);
                // Genertate walls
                Pixel pixel = bitmap->getPixel(i, j - 1); // get adjacent pixel
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate left wall
                    addFace(&indices, vertices.size(), false);
                    addVertices(&vertices, i, 0, true, true, false, j, texCoords);
                }
                pixel = bitmap->getPixel(i, j + 1);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate right wall
                    addFace(&indices, vertices.size(), true);
                    addVertices(&vertices, i, 0, true, true, false, (j + 1), texCoords);
                }
                pixel = bitmap->getPixel(i - 1, j);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate near wall
                    addFace(&indices, vertices.size(), true);
                    addVertices(&vertices, 0, j, false, true, true, i, texCoords);
                }
                pixel = bitmap->getPixel(i + 1, j);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    // Generate far wall
                    addFace(&indices, vertices.size(), false);
                    addVertices(&vertices, 0, j, false, true, true, (i + 1), texCoords);
                }
            }
        }
    }

    std::vector<Villain::Texture*> floorTextures = {Villain::ResourceManager::Instance()->loadTexture(tileAtlasFileName, "atlas")};
    material = new Villain::Material{"bricks", floorTextures, 8};
    mesh = new Villain::Mesh<VertexP1N1UV>(vertices, indices, floorTextures);
}

void Level::addFace(std::vector<unsigned int>* indices, int startLocation, bool direction) {
    if (direction) {
        indices->push_back(startLocation + 2);
        indices->push_back(startLocation + 1);
        indices->push_back(startLocation + 0);
        indices->push_back(startLocation + 3);
        indices->push_back(startLocation + 2);
        indices->push_back(startLocation + 0);
    } else {
        indices->push_back(startLocation + 0);
        indices->push_back(startLocation + 1);
        indices->push_back(startLocation + 2);
        indices->push_back(startLocation + 0);
        indices->push_back(startLocation + 2);
        indices->push_back(startLocation + 3);
    }
}

void Level::addVertices(std::vector<VertexP1N1UV>* vertices, int i, int j, bool x, bool y, bool z, float offset, float* texCoords) {
    if (x && z) { // floor on xz plane
        vertices->push_back({glm::vec3((i * ROOM_WIDTH), offset * ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[3])});
        vertices->push_back({glm::vec3(((i + 1) * ROOM_WIDTH), offset * ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[3])});
        vertices->push_back({glm::vec3(((i + 1) * ROOM_WIDTH), offset * ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[2])});
        vertices->push_back({glm::vec3((i * ROOM_WIDTH), offset * ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[2])});
    } else if (x && y) {
        vertices->push_back({glm::vec3((i * ROOM_WIDTH), (j * ROOM_HEIGHT), offset * ROOM_LENGTH), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[3])});
        vertices->push_back({glm::vec3(((i + 1) * ROOM_WIDTH), (j * ROOM_HEIGHT), offset * ROOM_LENGTH), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[3])});
        vertices->push_back({glm::vec3(((i + 1) * ROOM_WIDTH), (j + 1) * ROOM_HEIGHT, offset * ROOM_LENGTH), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[2])});
        vertices->push_back({glm::vec3((i * ROOM_WIDTH), (j + 1) * ROOM_HEIGHT, offset * ROOM_LENGTH), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[2])});
    } else if (y && z) {
        vertices->push_back({glm::vec3(offset * ROOM_WIDTH, i * ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[3])});
        vertices->push_back({glm::vec3(offset * ROOM_WIDTH, i * ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[3])});
        vertices->push_back({glm::vec3(offset * ROOM_WIDTH, (i + 1) * ROOM_HEIGHT, ((j + 1) * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[0], texCoords[2])});
        vertices->push_back({glm::vec3(offset * ROOM_WIDTH, (i + 1) * ROOM_HEIGHT, (j * ROOM_LENGTH)), glm::vec3(0.0f), glm::vec2(texCoords[1], texCoords[2])});
    } else {
        std::cerr << "Impossible plane combo!\n";
    }
}

float* Level::getTexCoords(int val) {
    int texX = val / NUM_TEXTURES;
    int texY = texX % NUM_TEX_EXP;
    texX /= NUM_TEX_EXP;

    float* res = new float[4];
    res[0] = 1.f - (float)texX / (float)NUM_TEX_EXP; // xHigher
    res[1] = res[0] - 1.f / NUM_TEX_EXP; // xLower
    res[3] = 1.f - (float)texY / (float)NUM_TEX_EXP; // yHigher
    res[2] = res[3] - 1.f / NUM_TEX_EXP; // yLower
    return res;
}
