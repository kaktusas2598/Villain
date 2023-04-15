#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera3D.hpp"
#include "rendering/DebugRenderer.hpp"

// Bullet Physics!
#include "btBulletDynamicsCommon.h"
#include "BulletDebugRenderer.hpp"

class Game : public Villain::Application {
    public:
        ~Game();
        void init();
        void handleEvents(float deltaTime);
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppRender(float deltaTime);

    private:
        Villain::Camera3D camera;

        void initBulletPhysics();
        void cleanupBulletPhysics();
        void createGround();
        void createRigidBody();

        // Bullet Physics objects
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

        btAlignedObjectArray<btCollisionShape*> collisionShapes;
        BulletDebugRenderer* bulletRenderer;
};

#endif // __GAME__
