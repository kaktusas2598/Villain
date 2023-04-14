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

#include "DebugConsole.hpp"
#include "rendering/DebugRenderer.hpp"

using namespace Villain;

Game::~Game() {
    cleanupBulletPhysics();
}

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    debugRenderer.init();

    // Add camera
    SceneNode* cam = (new SceneNode("Free look camera"))
            ->addComponent(new CameraComponent(&camera))
            ->addComponent(new MoveController())
            ->addComponent(new LookController());
    addToScene(cam);


    initBulletPhysics();

    // Create ground using bullet physics
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(250.), btScalar(1.), btScalar(250.)));

    collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -10, 0));

    btScalar groundMass(0.); // If mass is 0, body becomes static
    btVector3 localInertia(0, 0, 0);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo groundInfo(groundMass, groundMotionState, groundShape, localInertia);
    btRigidBody* groundBody = new btRigidBody(groundInfo);

    dynamicsWorld->addRigidBody(groundBody);
    /////////////////////////////

    //create a few dynamic rigidbodies
    // Re-using the same collision is better for memory usage and performance
    btBoxShape* boxShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
    collisionShapes.push_back(boxShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);
    boxShape->calculateLocalInertia(mass, localInertia);

    const int ARRAY_SIZE_X = 4;
    const int ARRAY_SIZE_Y = 4;
    const int ARRAY_SIZE_Z = 10;
    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                startTransform.setOrigin(btVector3(
                    btScalar(0.2 * i),
                    btScalar(20 + .2 * k),
                    btScalar(0.2 * j)));

                //createRigidBody(mass, startTransform, colShape);
                btDefaultMotionState* bodyMotionState = new btDefaultMotionState(startTransform);
                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, bodyMotionState, boxShape, localInertia);

                // Add bounciness!
                //rbInfo.m_restitution = 1.3f;
                //rbInfo.m_friction = 1.5f;

                btRigidBody* body = new btRigidBody(rbInfo);

                dynamicsWorld->addRigidBody(body);

            }
        }
    }


    // Create Bullet dynamic rigid body
    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    //collisionShapes.push_back(colShape);

    ///// Create Dynamic Objects
    //btTransform startTransform;
    //startTransform.setIdentity();

    //btScalar mass(1.f);
    //colShape->calculateLocalInertia(mass, localInertia);

    //startTransform.setOrigin(btVector3(50, 50, 0));

    ////using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    //btDefaultMotionState* bodyMotionState = new btDefaultMotionState(startTransform);
    //btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, bodyMotionState, colShape, localInertia);
    //btRigidBody* body = new btRigidBody(rbInfo);

    //dynamicsWorld->addRigidBody(body);
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

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }

        glm::vec3 pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
        debugRenderer.drawBox3D(pos, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    //debugRenderer.drawBox3D(glm::vec3(5.0f, 2.5f, -5.0f), glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), glm::vec3(0.1f, 5.0f, 10.0f));
    //debugRenderer.drawSphere(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec4(0.8f, 0.0f, 2.0f, 2.0f), 5.0f);
    //debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 10.f, 0.f));
    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);
}
