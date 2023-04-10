#include "Level.hpp"

#include "ResourceManager.hpp"
#include <iostream>
#include <sstream>

const float ROOM_WIDTH = 1.0f;
const float ROOM_LENGTH = 1.0f;
const float ROOM_HEIGHT = 2.0f;
const int NUM_TEX_EXP = 4;
const int NUM_TEXTURES = (int)pow(2, NUM_TEX_EXP);
const float DOOR_OPEN_MOVE_AMT = 0.9f;

Level::Level(const std::string& fileName, const std::string& tileAtlasFileName, Villain::Application* app) {
    application = app;
    bitmap = new Bitmap(fileName);
    generateLevel(tileAtlasFileName);
}

void Level::generateLevel(const std::string& tileAtlasFileName) {
    // Load and generate 3D map from bitmap texture
    levelNode = new Villain::SceneNode("Level 1");
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < bitmap->getWidth(); i++) {
        for (int j = 0; j < bitmap->getHeight(); j++) {
            Pixel pixel = bitmap->getPixel(i, j);
            if (pixel.R != 0 || pixel.G != 0 || pixel.B != 0) {
                float* texCoords = getTexCoords(pixel.G);

                // Objects like doors, enemies will be denoted by blue colour component in bitmap
                addSpecialObject(pixel.B, i, j);

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
    mesh = new Villain::Mesh<VertexP1N1UV>(vertices, indices);

    application->addToScene(levelNode->addComponent(new Villain::MeshRenderer<VertexP1N1UV>(mesh, *material)));
}

void Level::addDoor(int x, int y) {
    static unsigned int counter = 1; // static door counter
    std::stringstream doorName;
    doorName << "Door " << counter++;

    glm::vec3 openPosition =  glm::vec3(0.0f);
    Door* newDoor = new Door(openPosition);
    Villain::SceneNode* newNode = nullptr;
    Pixel bottomPixel = bitmap->getPixel(x, y - 1);
    if (bottomPixel.R == 0 && bottomPixel.G == 0 && bottomPixel.B == 0) {
        Villain::SceneNode* newNode = (new Villain::SceneNode(doorName.str(), glm::vec3(x + newDoor->Width + ROOM_WIDTH/2, 0.0f, y)))->addComponent(newDoor);
        newNode->getTransform()->setEulerRot(0.0f, 270.0f, 0.0f);
        openPosition = newNode->getTransform()->getPos() - glm::vec3(0.f, 0.f, DOOR_OPEN_MOVE_AMT);
        newDoor->setOpenPos(openPosition);
        levelNode->addChild(newNode);
    } else {
        newNode = (new Villain::SceneNode(doorName.str(), glm::vec3(x, 0.0f, y + ROOM_LENGTH/2)))->addComponent(newDoor);
        openPosition = newNode->getTransform()->getPos() - glm::vec3(DOOR_OPEN_MOVE_AMT, 0.f, 0.f);
        newDoor->setOpenPos(openPosition);
        levelNode->addChild(newNode);
    }
    doors.push_back(newDoor);
}

void Level::addSpecialObject(int blueValue, int x, int y) {
    if (blueValue == 16) {
        addDoor(x, y);
    }
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

glm::vec3 Level::checkCollisions(const glm::vec3& oldPos, const glm::vec3& newPos, float objectWidth, float objectLength) {
    glm::vec2 collision = glm::vec2(1.f); // Won't move in y axis, so only vec2 here
    glm::vec3 movement = newPos - oldPos;

    if (glm::length(movement)) {
        glm::vec2 blockSize(ROOM_WIDTH, ROOM_LENGTH);
        glm::vec2 objectSize(objectWidth, objectLength);

        glm::vec2 oldPos2(oldPos.x, oldPos.z);
        glm::vec2 newPos2(newPos.x, newPos.z);
        // Pretty bad way to check collision, need to improve so only checks nearby titles
        for (int i = 0; i < bitmap->getWidth(); i++) {
            for (int j = 0; j < bitmap->getHeight(); j++) {
                Pixel pixel = bitmap->getPixel(i, j);
                if (pixel.R == 0 && pixel.G == 0 && pixel.B == 0) {
                    glm::vec2 blockPos; // get pos for a wall block
                    blockPos.x = blockSize.x * i;
                    blockPos.y = blockSize.y * j;
                    glm::vec2 currentTileCol = rectCollide(oldPos2, newPos2, objectSize, blockSize, blockPos);
                    collision.x *= currentTileCol.x;
                    collision.y *= currentTileCol.y;
                }
            }
        }
        // Check door collision
        for(auto& door : doors){
            glm::vec2 doorPos(door->GetTransform()->getPos().x, door->GetTransform()->getPos().z);
            glm::vec2 doorSize = door->getSize();
            glm::vec2 doorCol = rectCollide(oldPos2, newPos2, objectSize, doorSize, doorPos);
            collision.x *= doorCol.x;
            collision.y *= doorCol.y;
        }

    }

    return glm::vec3(collision.x, 0.0f, collision.y);
}

glm::vec2 Level::rectCollide(glm::vec2& oldPos, glm::vec2& newPos, glm::vec2& objectSize, glm::vec2& blockSize, glm::vec2& blockPos) {
    glm::vec2 result(0.0f);

    if (newPos.x + objectSize.x < blockPos.x ||
        newPos.x - objectSize.x > blockPos.x + blockSize.x * blockSize.x ||
        oldPos.y + objectSize.y < blockPos.y ||
        oldPos.y - objectSize.y > blockPos.y + blockSize.y * blockSize.y) {
        result.x = 1.0f; // Collision found on X axis
    }
    if (oldPos.x + objectSize.x < blockPos.x ||
        oldPos.x - objectSize.x > blockPos.x + blockSize.x * blockSize.x ||
        newPos.y + objectSize.y < blockPos.y ||
        newPos.y - objectSize.y > blockPos.y + blockSize.y * blockSize.y) {
        result.y = 1.0f; // Collision found on Z axis
    }
    return result;
}
