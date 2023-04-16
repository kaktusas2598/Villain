#include "Game.hpp"

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

using namespace Villain;

Game::~Game() {
    delete physicsEngine;
}

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
    debugRenderer.init();

    physicsEngine = new BulletEngine({0.0, -9.8, 0.0});
    physicsEngine->setDebugMode(btIDebugDraw::DBG_NoDebug);

    createGround();

    addPlayer();

    addRigidBoxes();
}

// NOTE: Not sure about any of these parameters, justr trying to build character controller
void Game::addPlayer() {
    btCapsuleShape* capsuleShape = new btCapsuleShape(btScalar(0.5), btScalar(2.));
    physicsEngine->addCollisionShape(capsuleShape);

    btTransform playerTransform;
    playerTransform.setIdentity();
    btScalar playerMass(15.f);
    btVector3 localInertia(0, 0, 0);
    capsuleShape->calculateLocalInertia(playerMass, localInertia);
    playerTransform.setOrigin(btVector3(-60.0, 10, 0.0));

    btDefaultMotionState* bodyMotionState = new btDefaultMotionState(playerTransform);
    btRigidBody::btRigidBodyConstructionInfo playerBodyInfo(playerMass, bodyMotionState, capsuleShape, localInertia);

    btRigidBody* playerBody = new btRigidBody(playerBodyInfo);
    playerBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    BulletCharacterController* playerController = new BulletCharacterController(playerBody, capsuleShape);
    physicsEngine->addAction(playerController);
    physicsEngine->addRigidBody(playerBody);

    // Add camera/player node
    SceneNode* cam = (new SceneNode("Player"))
            ->addComponent(new CameraComponent(&camera))
            //->addComponent(new MoveController())
            ->addComponent(new LookController())
            ->addComponent(new BulletCharacterComponent(playerController));
    addToScene(cam);
}

void Game::createGround() {
    // Create mesh for ground
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_diff_4k.jpg", "redSandstone", GL_REPEAT)};
    Material mat("redSandstonePavement", textures, 8);
    MeshUtils::addTopFace(&vertices, &indices, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec2(250.0f));
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(250.), btScalar(.5), btScalar(250.)));
    physicsEngine->addCollisionShape(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -0.5, 0));

    btScalar groundMass(0.); // If mass is 0, body becomes static
    btVector3 localInertia(0, 0, 0);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo groundInfo(groundMass, groundMotionState, groundShape, localInertia);
    btRigidBody* groundBody = new btRigidBody(groundInfo);

    physicsEngine->addRigidBody(groundBody);
    // Add bodies to scene graph
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
    physicsEngine->addCollisionShape(boxShape);

    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);
    btVector3 localInertia(0, 0, 0);
    boxShape->calculateLocalInertia(mass, localInertia);

    const int ARRAY_SIZE_X = 8;
    const int ARRAY_SIZE_Y = 8;
    const int ARRAY_SIZE_Z = 8;
    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                startTransform.setOrigin(btVector3(
                    btScalar(0.2 * i),
                    btScalar(100 + .2 * k),
                    btScalar(0.2 * j)));

                //createRigidBody(mass, startTransform, colShape);
                btDefaultMotionState* bodyMotionState = new btDefaultMotionState(startTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, bodyMotionState, boxShape, localInertia);

                // Add bounciness!
                rbInfo.m_restitution = 0.2f;
                rbInfo.m_friction = 1.5f;

                btRigidBody* body = new btRigidBody(rbInfo);

                physicsEngine->addRigidBody(body);

                // Add bodies to scene graph
                SceneNode* bodyNode = (new SceneNode("Dynamic Body"))
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

void Game::onAppRender(float dt) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjMatrix();

    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);

    // Draw bullet physics
    physicsEngine->render(projection * view);
}
