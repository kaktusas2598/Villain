#include "Monster.hpp"

#include "Engine.hpp"
#include "ResourceManager.hpp"

using namespace Villain;

const float SCALE = 0.7f;
const float START = 0.0f;
const float SIZE_Y = SCALE * 2;
// Crazy constant to scale image and keep aspect ratio of monster sprite
const float SIZE_X = (float)((double)SIZE_Y / (1.9310344827586206896551724137931 * 2.0));

const float OFFSET_X = 0.05f;
const float OFFSET_Y = 0.01f;
const float TEX_MIN_X = -OFFSET_X;
const float TEX_MAX_X = 1 - OFFSET_X;
const float TEX_MIN_Y = -OFFSET_Y;
const float TEX_MAX_Y = 1 - OFFSET_Y;

Monster::Monster() : MeshRenderer<VertexP1N1UV>(nullptr, Material()) {

    material = Material{"enemySprite", std::vector<Texture*>{ResourceManager::Instance()->loadTexture(
            "assets/textures/SSWVA1.png", "enemy1")}, 8};
    std::vector<VertexP1N1UV> vertices;

    // NOTE: Now Mesh class only needs vertices and indices and it could contain methods to build common primitives like these Also need an easy way to generate UV coords in 3D, and calculate normals vertices.push_back({glm::vec3(Start, Start, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.f)}); vertices.push_back({glm::vec3(Start, Height, Start), glm::vec3(0.0f), glm::vec2(0.5f, 0.25f)});
    vertices.push_back({glm::vec3(-SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MAX_Y)});
    vertices.push_back({glm::vec3(-SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MAX_Y)});

    std::vector<unsigned int> indices{0, 1, 2, 0, 2, 3};

    mesh = new Mesh<VertexP1N1UV>(vertices, indices);
}

void Monster::update(float deltaTime) {
    // Monster always faces camera
    glm::vec3 directionToCamera = GetTransform()->getPos()
        - parent->getEngine()->getRenderingEngine()->getMainCamera()->getPosition();
    float angleToCameraFace = glm::degrees(atanf(directionToCamera.z / directionToCamera.x));
    if (directionToCamera.x > 0.0f) {
        angleToCameraFace += 180.0f;
    }
    GetTransform()->setEulerRot(0.0f, angleToCameraFace + 90.0f, 0.0f );

    switch (currentState) {
        case AIState::STATE_IDLE: idleUpdate(); break;
        case AIState::STATE_CHASE: chaseUpdate(); break;
        case AIState::STATE_ATTACK: attackUpdate(); break;
        case AIState::STATE_DYING: dyingUpdate(); break;
        case AIState::STATE_DEAD: deadUpdate(); break;
        default:
            std::cerr << "AI State not found!\n";
            break;
    }
}

void Monster::idleUpdate() {

}

void Monster::chaseUpdate() {

}

void Monster::attackUpdate() {

}

void Monster::dyingUpdate() {

}

void Monster::deadUpdate() {

}
