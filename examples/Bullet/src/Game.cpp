#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"

#include "rendering/DebugRenderer.hpp"
#include "BulletCharacterController.hpp"
#include "BulletCharacterComponent.hpp"

using namespace Villain;

Game::~Game() {
    cleanupBulletPhysics();
}

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
    debugRenderer.init();

    initBulletPhysics();

    createGround();

    addPlayer();

    addRigidBoxes();
}

void Game::initBulletPhysics() {
    // BULLET PHYSICS INITIALISATION CODE
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

    // Setup custom debug drawer
    bulletRenderer = new BulletDebugRenderer();
    dynamicsWorld->setDebugDrawer(bulletRenderer);
    dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb);
}

// NOTE: Not sure about any of these parameters, justr trying to build character controller
void Game::addPlayer() {
    // TEMP add character controller, try to create rigid body and shape
    btCapsuleShape* capsuleShape = new btCapsuleShape(btScalar(0.5), btScalar(2.));
    collisionShapes.push_back(capsuleShape);

    btTransform playerTransform;
    playerTransform.setIdentity();
    btScalar playerMass(5.f);
    btVector3 localInertia(0, 0, 0);
    capsuleShape->calculateLocalInertia(playerMass, localInertia);
    playerTransform.setOrigin(btVector3(-60.0, 30, 0.0));

    btDefaultMotionState* bodyMotionState = new btDefaultMotionState(playerTransform);
    btRigidBody::btRigidBodyConstructionInfo playerBodyInfo(playerMass, bodyMotionState, capsuleShape, localInertia);

    btRigidBody* playerBody = new btRigidBody(playerBodyInfo);
    playerBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    BulletCharacterController* playerController = new BulletCharacterController(playerBody, capsuleShape);
    dynamicsWorld->addAction(playerController);
    dynamicsWorld->addRigidBody(playerBody);

    // Add camera/player node
    SceneNode* cam = (new SceneNode("Player", glm::vec3(0.0f, 10.0f, 100.0f)))
            ->addComponent(new CameraComponent(&camera))
            //->addComponent(new MoveController())
            ->addComponent(new LookController())
            ->addComponent(new BulletCharacterComponent(playerController));
    addToScene(cam);
}

void Game::createGround() {
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(250.), btScalar(1.), btScalar(250.)));

    collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, 0, 0));

    btScalar groundMass(0.); // If mass is 0, body becomes static
    btVector3 localInertia(0, 0, 0);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo groundInfo(groundMass, groundMotionState, groundShape, localInertia);
    btRigidBody* groundBody = new btRigidBody(groundInfo);

    dynamicsWorld->addRigidBody(groundBody);
}

void Game::addRigidBoxes() {
    // Re-using the same collision for all boxes is better for memory usage and performance
    btBoxShape* boxShape = new btBoxShape(btVector3(btScalar(2.), btScalar(2.), btScalar(2.)));
    collisionShapes.push_back(boxShape);

    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);
    btVector3 localInertia(0, 0, 0);
    boxShape->calculateLocalInertia(mass, localInertia);

    const int ARRAY_SIZE_X = 5;
    const int ARRAY_SIZE_Y = 5;
    const int ARRAY_SIZE_Z = 5;
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
                rbInfo.m_restitution = 0.9f;
                rbInfo.m_friction = 1.5f;

                btRigidBody* body = new btRigidBody(rbInfo);

                dynamicsWorld->addRigidBody(body);
            }
        }
    }
}

void Game::cleanupBulletPhysics() {
    //cleanup in the reverse order of creation/initialization
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;

	collisionShapes.clear();
}

void Game::handleEvents(float deltaTime) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents(dt);
    // Update Bullet Physics simulation
    dynamicsWorld->stepSimulation(dt);
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

    // Rendering using custom renderer - recommended Bullet approach!
    dynamicsWorld->debugDrawWorld();
    bulletRenderer->render(projection * view, 1.0f);
}
