#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletDebugRenderer.hpp"

class BulletEngine {
    public:
        BulletEngine(const btVector3& gravity = {0.0, -9.81, 0.0});
        ~BulletEngine();

        void update(float deltaTime);
        void render(const glm::mat4& MVP);

        void initPhysics();
        void exitPhysics();

        // NOTE:: Transform probably would be better as param instead of just position
        btRigidBody* createRigidBody(
                btCollisionShape* collisionShape,
                bool newShape,
                const btVector3& position,
                btScalar mass,
                btScalar friction = 0.5,
                btScalar restituion = 0.);

        void addCollisionShape(btCollisionShape* shape) {
            collisionShapes.push_back(shape);
        }

        void addAction(btActionInterface* action) {
            dynamicsWorld->addAction(action);
        }

        inline void setDebugMode(int mode) {
            dynamicsWorld->getDebugDrawer()->setDebugMode(mode);
        }
    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

        btAlignedObjectArray<btCollisionShape*> collisionShapes;
        BulletDebugRenderer* bulletRenderer;
};
