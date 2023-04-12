#include "Gun.hpp"

#include "Engine.hpp"
#include "ResourceManager.hpp"

using namespace Villain;

const float GUN_OFFSET = -0.1325f;
const float SCALE = 0.1f;
const float START = 0.0f;
const float SIZE_Y = SCALE * 2;
// Crazy constant to scale image and keep aspect ratio of gun sprite
const float SIZE_X = (float)((double)SIZE_Y / (1.03797468354430379 * 2.0));

const float OFFSET_X = 0.0f;
const float OFFSET_Y = 0.0f;
const float TEX_MIN_X = -OFFSET_X;
const float TEX_MAX_X = 1 - OFFSET_X;
const float TEX_MIN_Y = -OFFSET_Y;
const float TEX_MAX_Y = 1 - OFFSET_Y;

Gun::Gun() : MeshRenderer<VertexP1N1UV>(nullptr, Villain::Material()) {
    material = Material{"gun", std::vector<Texture*>{ResourceManager::Instance()->loadTexture("assets/textures/PISGB0.png", "gun")}, 8};

    std::vector<VertexP1N1UV> vertices;
    vertices.push_back({glm::vec3(-SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MAX_Y)});
    vertices.push_back({glm::vec3(-SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MAX_Y)});

    std::vector<unsigned int> indices{0, 1, 2, 0, 2, 3};

    mesh = new Mesh<VertexP1N1UV>(vertices, indices);
}

void Gun::update(float deltaTime) {
    Camera3D* mainCam = dynamic_cast<Camera3D*>(parent->getEngine()->getRenderingEngine()->getMainCamera());
    GetTransform()->setPos(mainCam->getPosition() + mainCam->getFront() * 0.105f);
    GetTransform()->getPos().y += GUN_OFFSET;

    glm::vec3 directionToCamera = GetTransform()->getPos() - mainCam->getPosition();

    float angleToCameraFace = glm::degrees(atanf(directionToCamera.z / directionToCamera.x));
    if (directionToCamera.x < 0.0f) {
        angleToCameraFace += 180.0f;
    }
    GetTransform()->setEulerRot(0.0f, angleToCameraFace + 90.0f, 0.0f );
}

