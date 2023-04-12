#include "Medkit.hpp"
#include "Player.hpp"

#include "ResourceManager.hpp"

using namespace Villain;

const float PICKUP_DISTANCE = 1.5f;
const float HEAL_AMOUNT = 25.0f;

const float SCALE = 0.4f;
const float START = 0.0f;
const float SIZE_Y = SCALE;
// Crazy constant to scale image and keep aspect ratio of monster sprite
const float SIZE_X = (float)((double)SIZE_Y / (0.678571 * 4.0));

const float TEX_MIN_X = -0.0f;
const float TEX_MAX_X = 1.0f;
const float TEX_MIN_Y = -0.0f;
const float TEX_MAX_Y = 1.0f;

Medkit::Medkit(Level* level) : MeshRenderer<VertexP1N1UV>(nullptr, Material()), currentLevel(level) {

    material = Material{"medkit", std::vector<Texture*>{ResourceManager::Instance()->loadTexture("assets/textures/MEDIA0.png", "medkit")}, 8};
    std::vector<VertexP1N1UV> vertices;

    vertices.push_back({glm::vec3(-SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MAX_Y)});
    vertices.push_back({glm::vec3(-SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MAX_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, SIZE_Y, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MIN_Y)});
    vertices.push_back({glm::vec3(SIZE_X, START, START), glm::vec3(0.0f), glm::vec2(TEX_MIN_X, 1 - TEX_MAX_Y)});

    std::vector<unsigned int> indices{0, 1, 2, 0, 2, 3};

    mesh = new Mesh<VertexP1N1UV>(vertices, indices);
}

void Medkit::update(float deltaTime) {
    Camera3D* mainCam = dynamic_cast<Camera3D*>(parent->getEngine()->getRenderingEngine()->getMainCamera());

    glm::vec3 directionToCamera = mainCam->getPosition() - GetTransform()->getPos();

    float angleToCameraFace = glm::degrees(atanf(directionToCamera.z / directionToCamera.x));
    if (directionToCamera.x < 0.0f) {
        angleToCameraFace += 180.0f;
    }
    GetTransform()->setEulerRot(0.0f, angleToCameraFace + 90.0f, 0.0f );

    //if (directionToCamera.length() < PICKUP_DISTANCE) {
    if (glm::length(currentLevel->getPlayer()->GetTransform()->getPos() - GetTransform()->getPos()) < PICKUP_DISTANCE) {
        if (currentLevel->getPlayer()->getHealth() < currentLevel->getPlayer()->getMaxHealth()) {
            currentLevel->damagePlayer(-HEAL_AMOUNT);
            currentLevel->removeMedkit(this);
        }
    }
}
