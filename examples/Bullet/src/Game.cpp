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

using namespace Villain;

BulletEngine* Game::PhysicsWorld = nullptr;
Villain::SceneNode* Game::WorldNode = nullptr;

// Custom collision callback
// NOTE: Will get segfault if we collide with bodies without user pointer set
bool collisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
        const btCollisionObjectWrapper* obj2, int id2, int index2) {
    // NOTE: Engine wrapper class could have a vector of BulletBodyComponent to help access them as well
    BulletBodyComponent* graphComp = ((BulletBodyComponent*)obj1->getCollisionObject()->getUserPointer());
    ((BulletBodyComponent*)obj2->getCollisionObject()->getUserPointer())->Colliding = true;
    graphComp->Colliding = true;


    if (!graphComp->getBody()->isStaticObject()) {
        printf("Node collided: %s\n", graphComp->getParent()->getName().c_str());
        Game::WorldNode->removeChild(graphComp->getParent());
        Game::PhysicsWorld->getWorld()->removeCollisionObject(const_cast<btCollisionObject*>(obj1->getCollisionObject()));
    }
    // NOTE: Can set Colliding back to false after custom collision logic is used
    return true;
}

Game::~Game() {
    delete PhysicsWorld;
}

void Game::init() {
    WorldNode = new SceneNode("Physics World");
    // Register custom collision callback
    gContactAddedCallback = collisionCallback;

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

    PhysicsWorld = new BulletEngine({0.0, -9.8, 0.0});
    PhysicsWorld->setDebugMode(btIDebugDraw::DBG_NoDebug);

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

    btRigidBody* wallBody = PhysicsWorld->createRigidBody(new btBoxShape({btScalar(50.), btScalar(50.), btScalar(.5)}), true, {0, 0, 25}, 0.);
    BulletBodyComponent* wallComp = new BulletBodyComponent(wallBody);
    wallBody->setUserPointer(wallComp);
    SceneNode* wallNode = (new SceneNode("Wall"))
        ->addComponent(wallComp)
        ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    WorldNode->addChild(wallNode);
    /////////////////////////////////////////

    // TODO:
    // 5. Bullet utils class for common stuff like converting glm::vec3 to btVector3 etc.
    // 6. investigate constraints
    float s = 4;
    float h = 10;
    btSoftBody* cloth = btSoftBodyHelpers::CreatePatch(
            PhysicsWorld->getWorld()->getWorldInfo(),
            {-s, h, -s}, {s, h, -s}, {-s, h, s}, {s, h, s},
            50, 50, 4+8, true);
    // Fixes rendering problems, arbitrary value used
    cloth->m_cfg.viterations = 50;
    cloth->m_cfg.piterations = 50;
    cloth->setTotalMass(20.f);
    PhysicsWorld->getWorld()->addSoftBody(cloth);
    // TODO: Build Mesh using data from Bullet
    //cloth->m_faces
    //cloth->m_links

    btSoftBody* elipsoid = btSoftBodyHelpers::CreateEllipsoid(
            PhysicsWorld->getWorld()->getWorldInfo(),
            {0, 10, -10}, {3.f, 3.f, 3.f}, 1000.f);
    elipsoid->m_cfg.viterations = 50;
    elipsoid->m_cfg.piterations = 50;
    elipsoid->m_cfg.kPR = 10; // Change pressure
    cloth->setTotalMass(10.f);
    //cloth->setMass(0, 0); // Set 1st vertex(0) to be static (mass 0)
    PhysicsWorld->getWorld()->addSoftBody(elipsoid);

    indices.clear();
    vertices.clear();
    textures.clear();
    textures = {ResourceManager::Instance()->loadTexture("assets/textures/earth2048.bmp", "smallBlueDot")};
    Material earthMat("smallBlueDot", textures, 8);
    MeshUtils::addSphere(&vertices, &indices, 2.5f, glm::vec3(0.f, 0.f, 0.f));
    Mesh<VertexP1N1UV>* sphereMesh = new Mesh<VertexP1N1UV>(vertices, indices);
    btRigidBody* sphereBody = PhysicsWorld->createRigidBody(new btSphereShape(2.5f), true, {0, 50, 0}, btScalar(500.), btScalar(.5), 0.);
    BulletBodyComponent* sphereComp = new BulletBodyComponent(sphereBody);
    sphereBody->setUserPointer(sphereComp);
    SceneNode* ball = (new SceneNode("Ball"))
        ->addComponent(sphereComp)
        ->addComponent(new MeshRenderer<VertexP1N1UV>(sphereMesh, earthMat));
    WorldNode->addChild(ball);

    addToScene(WorldNode);
}

// NOTE: Not sure about any of these parameters, justr trying to build character controller
void Game::addPlayer() {
    btCapsuleShape* capsuleShape = new btCapsuleShape(btScalar(0.5), btScalar(2.));
    PhysicsWorld->addCollisionShape(capsuleShape);

    btRigidBody* playerBody = PhysicsWorld->createRigidBody(capsuleShape, false, {-5., 10.0, 2.0}, 15.);
    BulletCharacterController* playerController = new BulletCharacterController(playerBody, capsuleShape);
    PhysicsWorld->addAction(playerController);


    // Add camera/player node
    SceneNode* player = (new SceneNode("Player"))
            ->addComponent(new CameraComponent(&camera))
            //->addComponent(new MoveController())
            ->addComponent(new LookController())
            ->addComponent(new BulletCharacterComponent(playerController));
    WorldNode->addChild(player);
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

    btRigidBody* groundBody = PhysicsWorld->createRigidBody(new btBoxShape({btScalar(250.), btScalar(.5), btScalar(250.)}), true, {0, 0, 0}, 0.);
    groundBody->setCollisionFlags(groundBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

    // Add ground to scene graph
    BulletBodyComponent* groundComp = new BulletBodyComponent(groundBody);
    SceneNode* groundNode = (new SceneNode("Ground"))
        ->addComponent(groundComp)
        ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    groundBody->setUserPointer(groundComp);
    WorldNode->addChild(groundNode);
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
    PhysicsWorld->addCollisionShape(boxShape); // Adding shape to engine here and passing false to createRigidBody() below

    const int ARRAY_SIZE_X = 8;
    const int ARRAY_SIZE_Y = 8;
    const int ARRAY_SIZE_Z = 8;
    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                btRigidBody* body = PhysicsWorld->createRigidBody(boxShape, false,
                        {btScalar(30 + 0.2 * i), btScalar(100 + .2 * k), btScalar(0.2 * j)}
                        , 1., 1.5, 0.3);

                // Add bodies to scene graph
                BulletBodyComponent* boxComp = new BulletBodyComponent(body);
                body->setUserPointer(boxComp);
                SceneNode* bodyNode = (new SceneNode("Dynamic Body " + std::to_string(j)))
                    ->addComponent(boxComp)
                    ->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
                WorldNode->addChild(bodyNode);
            }
        }
    }
}

void Game::handleEvents(float deltaTime) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }

    if (InputManager::Instance()->isKeyPressed(SDLK_f)) {
        shootSphere();
    }

    // Bullet debug modes
    if (InputManager::Instance()->isKeyDown(SDLK_1)) {
        PhysicsWorld->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_2)) {
        PhysicsWorld->setDebugMode(btIDebugDraw::DBG_DrawAabb);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_3)) {
        PhysicsWorld->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_0)) {
        PhysicsWorld->setDebugMode(btIDebugDraw::DBG_NoDebug);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents(dt);
    // Update Bullet Physics simulation
    PhysicsWorld->update(dt);
}

// Gets mouse ray direction vector in world space
glm::vec3 Game::mouseRayToWorld(const glm::vec2 & pos) {
    // Normalised device coords
    float x = (2.0f * pos.x) / Engine::getScreenWidth() - 1.0f;
    float y = 1.0f - (2.0f * pos.y) / Engine::getScreenHeight();
    // Homogeneous clip coords
    glm::vec4 rayClip = {x, y, -1.0f, 1.0f};
    // Camera/eye/view coords
    glm::vec4 rayEye = glm::inverse(camera.getProjMatrix()) * rayClip;
    rayEye = {rayEye.x, rayEye.y, -1.0f, 0.0f};
    // World coords
    glm::vec3 rayWorld = glm::vec3(glm::inverse(camera.getViewMatrix()) * rayEye);
    return glm::normalize(rayWorld);
}

void Game::shootSphere() {
    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraFront = camera.getFront();
    glm::vec3 startPos = cameraPos + camera.getFront() * 5.f;
    btVector3 ballStartPos = {startPos.x, startPos.y, startPos.z};

    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/moon1024.bmp", "moon")};
    Material moonMat("moon", textures, 8);
    MeshUtils::addSphere(&vertices, &indices, 0.5f, glm::vec3(0.f, 0.f, 0.f));
    Mesh<VertexP1N1UV>* sphereMesh = new Mesh<VertexP1N1UV>(vertices, indices);
    btRigidBody* sphereBody = PhysicsWorld->createRigidBody(new btSphereShape(0.5f), true, ballStartPos, btScalar(50.), btScalar(.5), 0.);
    BulletBodyComponent* sphereComp = new BulletBodyComponent(sphereBody);
    sphereBody->setUserPointer(sphereComp);
    SceneNode* ball = (new SceneNode("Ball"))
        ->addComponent(sphereComp)
        ->addComponent(new MeshRenderer<VertexP1N1UV>(sphereMesh, moonMat));
    WorldNode->addChild(ball);

    // Set callision flags for custom callback
    sphereBody->setCollisionFlags(sphereBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    glm::vec3 cameraTo = cameraFront * 20.0f;
    btVector3 to = {cameraTo.x, cameraTo.y, cameraTo.z};
    sphereBody->setLinearVelocity(to);
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
    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraFront = camera.getFront();

    glm::vec2 mouseCoords = InputManager::Instance()->getMouseCoords();
    glm::vec3 mouseWorld = mouseRayToWorld(mouseCoords);
    //printf("Mouse screen coords: %f.3, %f.3\n", mouseCoords.x, mouseCoords.y);
    //printf("Mouse World Coords: %f.3, %f.3, %f.3\n", mouseWorld.x, mouseWorld.y, mouseWorld.z);
    debugRenderer.drawLine(cameraPos, cameraPos + mouseWorld * 1000.0f, glm::vec4(1.0f));


    //printf("Camera pos: %f.3, %f.3, %f.3\n", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    //printf("Camera front: %f.3, %f.3, %f.3\n", camera.getFront().x, camera.getFront().y, camera.getFront().z);
    glm::vec3 cameraTo = cameraPos + cameraFront * 1000.0f;
    //glm::vec3 cameraTo = cameraPos + mouseWorld * 1000.0f;
    btVector3 from = {cameraPos.x, cameraPos.y, cameraPos.z};
    btVector3 to = {cameraTo.x, cameraTo.y, cameraTo.z};

    // Get first ray hit
    btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
    closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    PhysicsWorld->getWorld()->rayTest(from, to, closestResults);

    if (closestResults.hasHit()) {
		//closestResults.m_collisionObject->getUserPointer()
		//PhysicsWorld->getWorld()->removeCollisionObject(const_cast<btCollisionObject*>(closestResults.m_collisionObject));

        //closestResults.m_collisionObject->getCollisionShape()->getShapeType()
        //btRigidBody* body = btRigidBody::upcast(obj1);
        debugRenderer.drawLine(cameraPos, cameraTo, glm::vec4(0.0f, 1.f, 0.f, 1.0));

        btVector3 p = from.lerp(to, closestResults.m_closestHitFraction);
        debugRenderer.drawLine(cameraPos, {p.getX(), p.getY(), p.getZ()}, glm::vec4(1.0f, 0.f, 0.f, 1.0));
        PhysicsWorld->getWorld()->getDebugDrawer()->drawSphere(p, 0.1, {0, 0, 1});
        PhysicsWorld->getWorld()->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, {0, 0, 1});
    }
    /////////

    debugRenderer.end();
    debugRenderer.render(projection * view  , 2.0f);

    // Not neccessary, just making sure for now all soft bodies are drawn even not in debug mode
    for (int i = 0; i < PhysicsWorld->getWorld()->getSoftBodyArray().size(); i++) {
        btSoftBody* psb = (btSoftBody*)PhysicsWorld->getWorld()->getSoftBodyArray()[i];
        btSoftBodyHelpers::DrawFrame(psb, PhysicsWorld->getWorld()->getDebugDrawer());
        btSoftBodyHelpers::Draw(psb, PhysicsWorld->getWorld()->getDebugDrawer(), PhysicsWorld->getWorld()->getDrawFlags());
    }

    // 2nd render batch for screen space crosshair display
    const float CROSSHAIR_SIZE = 0.05f;
    debugRenderer.drawLine(glm::vec3(-CROSSHAIR_SIZE, 0.0f, 0.0f), glm::vec3(CROSSHAIR_SIZE, 0.0f, 0.0f), glm::vec4(1.0f));
    debugRenderer.drawLine(glm::vec3(0.0f, -CROSSHAIR_SIZE, 0.0f), glm::vec3(0.0f, CROSSHAIR_SIZE, 0.0f), glm::vec4(1.0f));
    debugRenderer.end();
    debugRenderer.render(glm::mat4(1.0f), 2.0f);

    // Draw bullet physics
    PhysicsWorld->render(projection * view);

    skybox->render(projection, view);
}
