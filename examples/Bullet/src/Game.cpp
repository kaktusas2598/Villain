#include "Game.hpp"

#include "BulletSoftBody/btSoftBody.h"
#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/MoveController.hpp"

#include "rendering/DebugRenderer.hpp"
#include "rendering/MeshUtils.hpp"
#include "BulletBodyComponent.hpp"
#include "BulletCharacterController.hpp"
#include "BulletCharacterComponent.hpp"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

using namespace Villain;

Game::~Game() {
    delete physicsEngine;
}

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
    debugRenderer.init();

    std::vector<std::string> faces{
        "assets/textures/skybox/posx.jpg",
        "assets/textures/skybox/negx.jpg",
        "assets/textures/skybox/posy.jpg",
        "assets/textures/skybox/negy.jpg",
        "assets/textures/skybox/posz.jpg",
        "assets/textures/skybox/negz.jpg"
    };

    skybox = std::make_unique<Villain::SkyBox>(faces, "assets/shaders/cubemap.glsl");


    physicsEngine = new BulletEngine({0.0, -9.8, 0.0});
    physicsEngine->setDebugMode(btIDebugDraw::DBG_NoDebug);

    createGround();

    addPlayer();

    addRigidBoxes();

    //////////////////////////////////////////
    /// TEMP TEST code gor MeshUtils planes
    // Create mesh for wall
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_diff_4k.jpg", "redSandstone", GL_REPEAT)};
    Material mat("redSandstonePavement", textures, 8);
    float uvCoords[4] = {0.0f, 100.0f, 0.0f, 100.0f};
    MeshUtils::addXYPlane(&vertices, &indices, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec2(50.0f), uvCoords, false);
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    btRigidBody* wallBody = physicsEngine->createRigidBody(new btBoxShape({btScalar(50.), btScalar(50.), btScalar(.5)}), true, {0, 0, 25}, 0.);
    SceneNode* wallNode = (new SceneNode("Ground"))
        ->addComponent(new BulletBodyComponent(wallBody))
        ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    addToScene(wallNode);
    /////////////////////////////////////////

    // TODO:
    // 1. implement casting rays, add example, like shooting projectiles for example
    // 3. investigate soft bodies
    // 4. collision callbacks, identify specific bodies
    // 5. Bullet utils class for common stuff like converting glm::vec3 to btVector3 etc.

    // TODO: BulletEngine class needs to support bt soft world
    //btSoftBody* cloth = btSoftBodyHelpers::CreatePatch(
            //physicsEngine->getWorld()->getWorldInfo(), {}, {}, {}, {}, 1, 1, 1, false);

    indices.clear();
    vertices.clear();
    textures.clear();
    textures = {ResourceManager::Instance()->loadTexture("assets/textures/earth2048.bmp", "smallBlueDot")};
    Material earthMat("smallBlueDot", textures, 8);
    MeshUtils::addSphere(&vertices, &indices, 2.5f, glm::vec3(0.f, 0.f, 0.f));
    Mesh<VertexP1N1UV>* sphereMesh = new Mesh<VertexP1N1UV>(vertices, indices);
    btRigidBody* sphereBody = physicsEngine->createRigidBody(new btSphereShape(2.5f), true, {0, 50, 0}, btScalar(500.), btScalar(.5), 0.);
    SceneNode* ball = (new SceneNode("Ball"))
        ->addComponent(new BulletBodyComponent(sphereBody))
        ->addComponent(new MeshRenderer<VertexP1N1UV>(sphereMesh, earthMat));
    addToScene(ball);

}

// NOTE: Not sure about any of these parameters, justr trying to build character controller
void Game::addPlayer() {
    btCapsuleShape* capsuleShape = new btCapsuleShape(btScalar(0.5), btScalar(2.));
    physicsEngine->addCollisionShape(capsuleShape);

    btRigidBody* playerBody = physicsEngine->createRigidBody(capsuleShape, false, {-5., 10.0, 2.0}, 15.);
    BulletCharacterController* playerController = new BulletCharacterController(playerBody, capsuleShape);
    physicsEngine->addAction(playerController);


    // Add camera/player node
    SceneNode* player = (new SceneNode("Player"))
            ->addComponent(new CameraComponent(&camera))
            //->addComponent(new MoveController())
            ->addComponent(new LookController())
            ->addComponent(new BulletCharacterComponent(playerController));
    addToScene(player);
}

void Game::createGround() {
    // Create mesh for ground
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_diff_4k.jpg", "redSandstone", GL_REPEAT)};
    Material mat("redSandstonePavement", textures, 8);
    float uvCoords[4] = {0.0f, 500.0f, 0.0f, 500.0f};
    MeshUtils::addXZPlane(&vertices, &indices, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec2(250.0f), uvCoords, false);
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    btRigidBody* groundBody = physicsEngine->createRigidBody(new btBoxShape({btScalar(250.), btScalar(.5), btScalar(250.)}), true, {0, 0, 0}, 0.);

    // Add ground to scene graph
    SceneNode* groundNode = (new SceneNode("Ground"))
        ->addComponent(new BulletBodyComponent(groundBody))
        ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    addToScene(groundNode);
}

void Game::addRigidBoxes() {
    // Create common mesh for all boxes
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    MeshUtils::addAABB(&vertices, &indices);
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate")};
    Material mat("cartoonWood", textures, 8);

    // Re-using the same collision for all boxes is better for memory usage and performance
    btBoxShape* boxShape = new btBoxShape({0.5, 0.5, 0.5});
    physicsEngine->addCollisionShape(boxShape); // Adding shape to engine here and passing false to createRigidBody() below

    const int ARRAY_SIZE_X = 8;
    const int ARRAY_SIZE_Y = 8;
    const int ARRAY_SIZE_Z = 8;
    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                btRigidBody* body = physicsEngine->createRigidBody(boxShape, false,
                        {btScalar(30 + 0.2 * i), btScalar(100 + .2 * k), btScalar(0.2 * j)}
                        , 1., 1.5, 0.3);

                // Add bodies to scene graph
                SceneNode* bodyNode = (new SceneNode("Dynamic Body " + std::to_string(j)))
                    ->addComponent(new BulletBodyComponent(body))
                    ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
                addToScene(bodyNode);
            }
        }
    }
}

void Game::handleEvents(float deltaTime) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }

    // Bullet debug modes
    if (InputManager::Instance()->isKeyDown(SDLK_1)) {
        physicsEngine->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_2)) {
        physicsEngine->setDebugMode(btIDebugDraw::DBG_DrawAabb);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_3)) {
        physicsEngine->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_0)) {
        physicsEngine->setDebugMode(btIDebugDraw::DBG_NoDebug);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents(dt);
    // Update Bullet Physics simulation
    physicsEngine->update(dt);
}

glm::vec3 Game::getRayFromScreenSpace(const glm::vec2 & pos) {
    const float halfScreenW = Engine::getScreenWidth()/2.f;
    const float halfScreenH = Engine::getScreenHeight()/2.f;

    glm::mat4 invMat= glm::inverse(camera.getProjMatrix() * camera.getViewMatrix());
    glm::vec4 near = glm::vec4((pos.x - halfScreenW) / halfScreenW, -1*(pos.y - halfScreenH) / halfScreenH, -1, 1.0);
    glm::vec4 far = glm::vec4((pos.x - halfScreenH) / halfScreenW, -1*(pos.y - halfScreenH) / halfScreenH, 1, 1.0);
    glm::vec4 nearResult = invMat*near;
    glm::vec4 farResult = invMat*far;
    nearResult /= nearResult.w;
    farResult /= farResult.w;
    glm::vec3 dir = glm::vec3(farResult - nearResult );
    return normalize(dir);
}

void Game::onAppRender(float dt) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjMatrix();

    // Draw coordinate gizmo: XYZ axis and little arrows for each axis
    debugRenderer.drawLine(glm::vec3(0.f, 1.f, 0.f), glm::vec3(5.f, 1.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(4.5f, 1.f, -0.5f), glm::vec3(5.f, 1.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(4.5f, 1.f, 0.5f), glm::vec3(5.f, 1.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    debugRenderer.drawLine(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(-0.5f, 4.5f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.5f, 4.5f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    debugRenderer.drawLine(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(-0.5f, 1.f, 4.5f), glm::vec3(0.f, 1.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.5f, 1.f, 4.5f), glm::vec3(0.f, 1.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));


    /////////
    // Get first ray hit
    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraFront = camera.getFront();
    //printf("Camera pos: %f.3, %f.3, %f.3\n", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    //printf("Camera front: %f.3, %f.3, %f.3\n", camera.getFront().x, camera.getFront().y, camera.getFront().z);
    cameraPos.y = 1.0f;
    //cameraFront.y = 0.7f;
    glm::vec3 cameraTo = cameraPos + cameraFront * 1000.0f;
    btVector3 from = {cameraPos.x, cameraPos.y, cameraPos.z};
    btVector3 to = {cameraTo.x, cameraTo.y, cameraTo.z};

    btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
    closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    physicsEngine->getWorld()->rayTest(from, to, closestResults);

    //glm::vec3 mouseRay = getRayFromScreenSpace(InputManager::Instance()->getMouseCoords());
    //printf("Mouse ray: %f.3, %f.3, %f.3\n", mouseRay.x, mouseRay.y, mouseRay.z);
    //debugRenderer.drawLine(cameraPos, mouseRay, glm::vec4(0.0f, 0.1f, 1.f, 1.0));

    if (closestResults.hasHit()) {
        //closestResults.m_collisionObject->getCollisionShape()->getShapeType()
        debugRenderer.drawLine(cameraPos, cameraTo, glm::vec4(0.0f, 1.f, 0.f, 1.0));

        btVector3 p = from.lerp(to, closestResults.m_closestHitFraction);
        debugRenderer.drawLine(cameraPos, {p.getX(), p.getY(), p.getZ()}, glm::vec4(1.0f, 0.f, 0.f, 1.0));
        physicsEngine->getWorld()->getDebugDrawer()->drawSphere(p, 0.1, {0, 0, 1});
        physicsEngine->getWorld()->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, {0, 0, 1});
    }
    /////////


    debugRenderer.end();
    debugRenderer.render(projection * view  , 2.0f);

    // 2nd render batch for screen space crosshair display
    const float CROSSHAIR_SIZE = 0.05f;
    debugRenderer.drawLine(glm::vec3(-CROSSHAIR_SIZE, 0.0f, 0.0f), glm::vec3(CROSSHAIR_SIZE, 0.0f, 0.0f), glm::vec4(1.0f));
    debugRenderer.drawLine(glm::vec3(0.0f, -CROSSHAIR_SIZE, 0.0f), glm::vec3(0.0f, CROSSHAIR_SIZE, 0.0f), glm::vec4(1.0f));
    debugRenderer.end();
    //debugRenderer.drawLine({0, -1, 0}, mouseRay, glm::vec4(0.0f, 0.1f, 1.f, 1.0));
    debugRenderer.render(glm::mat4(1.0f), 2.0f);

    // Draw bullet physics
    physicsEngine->render(projection * view);

    skybox->render(projection, view);
}
