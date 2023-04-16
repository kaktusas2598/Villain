#include "BulletEngine.hpp"

BulletEngine::BulletEngine(const btVector3& gravity) {
    initPhysics();
    dynamicsWorld->setGravity(gravity);
}
BulletEngine::~BulletEngine() {
    exitPhysics();
}

void BulletEngine::update(float deltaTime) {
    dynamicsWorld->stepSimulation(deltaTime);
}

void BulletEngine::render(const glm::mat4& MVP) {
    // Rendering using custom renderer - recommended Bullet approach!
    dynamicsWorld->debugDrawWorld();
    bulletRenderer->render(MVP, 1.0f);
}

void BulletEngine::initPhysics() {
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

    // Setup custom debug drawer
    bulletRenderer = new BulletDebugRenderer();
    dynamicsWorld->setDebugDrawer(bulletRenderer);
    dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawAabb);
}

void BulletEngine::exitPhysics() {
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
