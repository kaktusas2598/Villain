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
#include "BulletUtils.hpp"

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

    camera = new Camera();
    camera->setZPlanes(0.1f, 1000.f); // for bigger render range
    camera->rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
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

    // LIGHTING
    glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.0f);
    //glm::vec3 greenLight = glm::vec3(0.0f, 1.0f, 0.0f);
    //pointLight1 = new PointLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f),glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(1.0f, 0.022f, 0.0019f));
    //SceneNode* point = ((new SceneNode("Point Light 1", glm::vec3(0.f, 5.f, 10.f)))->addComponent(pointLight1));
    //WorldNode->addChild(point);
    pointLight2 = new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(1.0f, 0.022f, 0.0019f));
    SceneNode* point2 = ((new SceneNode("Point Light 2", glm::vec3(-20.f, 5.f, 10.f)))->addComponent(pointLight2));
    WorldNode->addChild(point2);
    //pointLight3 = new PointLight(greenLight * glm::vec3(0.2f), greenLight, glm::vec3(1.0f),glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(1.0f, 0.022f, 0.0019f));
    //SceneNode* point3 = ((new SceneNode("Point Light 3", glm::vec3(20.f, 5.f, 10.f)))->addComponent(pointLight3));
    //WorldNode->addChild(point3);

    flashlight = new SpotLight(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(10.f, 10.f, 0.f), glm::vec3(0.9f, -0.8f, -0.1f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.f, 0.022f, 0.0019f));//, &camera);
    SceneNode* spotLight = ((new SceneNode("Spot Light", glm::vec3(10., 10., 5.)))->addComponent(flashlight));
    WorldNode->addChild(spotLight);


    //directionalLight = new DirectionalLight(glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0f),glm::vec3(-0.2f, -1.0f, -0.3f));
    //SceneNode* dirLight = ((new SceneNode("Directional Light 1", glm::vec3(-25, 20, 0)))
            //->addComponent(directionalLight));
    //WorldNode->addChild(dirLight);
    //////////////////////

    PhysicsWorld = new BulletEngine({0.0, -9.8, 0.0});
    PhysicsWorld->setDebugMode(btIDebugDraw::DBG_NoDebug);

    createGround();

    addPlayer();

    addRigidBoxes();

    //////////////////////////////////////////
    /// TEMP TEST code gor MeshUtils planes
    // Create mesh for wall
    std::vector<VertexP1N1T1B1UV> vertices;
    std::vector<unsigned int> indices;
    Material mat(
            "redSandstonePavement",
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_diff_4k.jpg", "redSandstone", GL_REPEAT),
            128, nullptr,
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_nor_gl_4k.jpg", "redSandstoneNormal", GL_REPEAT),
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_disp_4k.jpg", "redSandstoneDisp", GL_REPEAT)
            );
    float uvCoords[4] = {0.0f, 100.0f, 0.0f, 100.0f};
    MeshUtils<VertexP1N1T1B1UV>::addXYPlane(&vertices, &indices, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec2(50.0f), uvCoords, false);
    MeshUtils<VertexP1N1T1B1UV>::addTangents(&vertices, &indices);
    Mesh<VertexP1N1T1B1UV>* mesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);

    btRigidBody* wallBody = PhysicsWorld->createRigidBody(new btBoxShape({btScalar(50.), btScalar(50.), btScalar(.5)}), true, {0, 0, 25}, 0.);
    BulletBodyComponent* wallComp = new BulletBodyComponent(wallBody);
    wallBody->setUserPointer(wallComp);
    SceneNode* wallNode = (new SceneNode("Wall"))
        ->addComponent(wallComp)
        ->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(mesh, mat));
    WorldNode->addChild(wallNode);
    /////////////////////////////////////////

    // TODO:
    // 5. Bullet utils class for common stuff like converting glm::vec3 to btVector3 etc.
    // 6. investigate constraints

    // SOFT BODIES
    // TODO: TEMP commented while testing shadow mapping!
    /*float s = 4;
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

    // Generate mesh for soft body
    // TODO: Finish this, not working yet
    indices.clear();
    vertices.clear();
    vertices.resize(cloth->m_faces.size() * 3);
    indices.reserve(cloth->m_faces.size() * 3);
    printf("Cloth face count: %i\n", cloth->m_faces.size());
    for (int i = 0; i < cloth->m_faces.size(); i++) {
        indices.push_back(3 * i);
        indices.push_back(3 * i + 1);
        indices.push_back(3 * i + 2);

        // TODO: UV?
        vertices.push_back({BulletUtils::toGlm(cloth->m_faces[i].m_n[0]->m_x), BulletUtils::toGlm(cloth->m_faces[i].m_n[0]->m_n), glm::vec2(0.0f)});
        vertices.push_back({BulletUtils::toGlm(cloth->m_faces[i].m_n[1]->m_x), BulletUtils::toGlm(cloth->m_faces[i].m_n[1]->m_n), glm::vec2(0.5f)});
        vertices.push_back({BulletUtils::toGlm(cloth->m_faces[i].m_n[2]->m_x), BulletUtils::toGlm(cloth->m_faces[i].m_n[2]->m_n), glm::vec2(1.0f)});
    }
    // Add soft body to scene graph
    Mesh<VertexP1N1T1B1UV>* clothMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);
    //BulletBodyComponent* clothComp = new BulletBodyComponent(cloth);
    //wallBody->setUserPointer(wallComp);
    SceneNode* clothNode = (new SceneNode("Cloth"))
        //->addComponent(wallComp)
        ->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(clothMesh, mat));
    WorldNode->addChild(clothNode);


    btSoftBody* elipsoid = btSoftBodyHelpers::CreateEllipsoid(
            PhysicsWorld->getWorld()->getWorldInfo(),
            {0, 10, -10}, {3.f, 3.f, 3.f}, 1000.f);
    elipsoid->m_cfg.viterations = 50;
    elipsoid->m_cfg.piterations = 50;
    elipsoid->m_cfg.kPR = 10; // Change pressure
    elipsoid->setTotalMass(10.f);
    elipsoid->setMass(0, 0); // Set 1st vertex(0) to be static (mass 0)
    PhysicsWorld->getWorld()->addSoftBody(elipsoid);*/

    //btSoftBody* rope = btSoftBodyHelpers::CreateRope(
            //PhysicsWorld->getWorld()->getWorldInfo(),
            //{10., 5, -10},
            //{10., 5, 10},
            //50, 4+8);
    //rope->m_cfg.viterations = 50;
    //rope->m_cfg.piterations = 50;
    //rope->setTotalMass(5.f);
    //PhysicsWorld->getWorld()->addSoftBody(rope);
    ////////////////////////

    indices.clear();
    vertices.clear();
    Material earthMat("smallBlueDot", ResourceManager::Instance()->loadTexture("assets/textures/earth2048.bmp", "smallBlueDot"), 8);
    MeshUtils<VertexP1N1T1B1UV>::addSphere(&vertices, &indices, 2.5f, glm::vec3(0.f, 0.f, 0.f));
    //FIXME: only half of sphere looks correct with tangents
    MeshUtils<VertexP1N1T1B1UV>::addTangents(&vertices, &indices);
    Mesh<VertexP1N1T1B1UV>* sphereMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);
    btRigidBody* sphereBody = PhysicsWorld->createRigidBody(new btSphereShape(2.5f), true, {0, 50, 0}, btScalar(500.), btScalar(.5), 0.);
    BulletBodyComponent* sphereComp = new BulletBodyComponent(sphereBody);
    sphereBody->setUserPointer(sphereComp);
    SceneNode* ball = (new SceneNode("Ball"))
        ->addComponent(sphereComp)
        ->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(sphereMesh, earthMat));
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
            ->addComponent(new CameraComponent(camera))
            //->addComponent(new MoveController())
            ->addComponent(new LookController())
            ->addComponent(new BulletCharacterComponent(playerController));
    WorldNode->addChild(player);
}

void Game::createGround() {
    // Create mesh for ground
    std::vector<VertexP1N1T1B1UV> vertices;
    std::vector<unsigned int> indices;
    Material mat(
            "redSandstonePavement",
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_diff_4k.jpg", "redSandstone", GL_REPEAT),
            128, nullptr,
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_nor_gl_4k.jpg", "redSandstoneNormal", GL_REPEAT),
            ResourceManager::Instance()->loadTexture("assets/textures/red_sandstone_pavement_disp_4k.jpg", "redSandstoneDisp", GL_REPEAT)
            );
    float uvCoords[4] = {0.0f, 500.0f, 0.0f, 500.0f};
    MeshUtils<VertexP1N1T1B1UV>::addXZPlane(&vertices, &indices, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec2(250.0f), uvCoords, false);
    MeshUtils<VertexP1N1T1B1UV>::addTangents(&vertices, &indices);
    Mesh<VertexP1N1T1B1UV>* mesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);

    btRigidBody* groundBody = PhysicsWorld->createRigidBody(new btBoxShape({btScalar(250.), btScalar(.5), btScalar(250.)}), true, {0, 0, 0}, 0.);
    groundBody->setCollisionFlags(groundBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

    // Add ground to scene graph
    BulletBodyComponent* groundComp = new BulletBodyComponent(groundBody);
    SceneNode* groundNode = (new SceneNode("Ground"))
        ->addComponent(groundComp)
        ->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(mesh, mat));
    groundBody->setUserPointer(groundComp);
    WorldNode->addChild(groundNode);
}

void Game::addRigidBoxes() {
    // Create common mesh for all boxes
    std::vector<VertexP1N1T1B1UV> vertices;
    std::vector<unsigned int> indices;
    MeshUtils<VertexP1N1T1B1UV>::addAABB(&vertices, &indices);
    MeshUtils<VertexP1N1T1B1UV>::addTangents(&vertices, &indices);
    Mesh<VertexP1N1T1B1UV>* mesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);

    //std::vector<Texture*> textures = {ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate")};
    Material mat("cartoonWood",
            ResourceManager::Instance()->loadTexture("assets/textures/woodDiffuse.jpg", "crateBase"),
            128, nullptr,
            ResourceManager::Instance()->loadTexture("assets/textures/woodNormal.jpg", "crateNormal"));

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
                    ->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(mesh, mat));
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

void Game::shootSphere() {
    glm::vec3 cameraPos = camera->getPosition();
    glm::vec3 cameraFront = camera->getFront();
    glm::vec3 startPos = cameraPos + camera->getFront() * 5.f;
    btVector3 ballStartPos = {startPos.x, startPos.y, startPos.z};

    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    Material moonMat("lua", ResourceManager::Instance()->loadTexture("assets/textures/moon1024.bmp", "lua"), 8);
    MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, 0.5f, glm::vec3(0.f, 0.f, 0.f));
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
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjMatrix();

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

    // Directional light has no position, but we need to set some sort of position for shadow map! So this
    // is just a representation os such position
    //debugRenderer.drawSphere(directionalLight->GetTransform()->getPos(), 1.0f, glm::vec4(0.8f, 1.0f, 0.0f, 1.0f));
    //debugRenderer.drawLine(directionalLight->GetTransform()->getPos(), (directionalLight->GetTransform()->getPos() + directionalLight->Direction * 3.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));


    // Spot light direction
    //debugRenderer.drawLine(flashlight->Position, flashlight->Position + (5.f * flashlight->Direction), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    //debugRenderer.drawSphere(flashlight->GetTransform()->getPos(), 1.0f, glm::vec4(0.8f, 1.0f, 0.0f, 1.0f));

    // Point lights
    debugRenderer.drawSphere(pointLight2->GetTransform()->getPos(), 1.0f, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));

    /////////
    glm::vec3 cameraPos = camera->getPosition();
    glm::vec3 cameraFront = camera->getFront();

    glm::vec2 mouseCoords = InputManager::Instance()->getMouseCoords();
    glm::vec3 mouseWorld = camera->mouseRayToWorld(mouseCoords);
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
