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

        void addCollisionShape(btCollisionShape* shape) {
            collisionShapes.push_back(shape);
        }

        void addAction(btActionInterface* action) {
            dynamicsWorld->addAction(action);
        }

        // TODO: Engine should also be able to construct body itself
        void addRigidBody(btRigidBody* rigidBody) {
            dynamicsWorld->addRigidBody(rigidBody);
        }

        inline void setDebugMode(bool mode) { debugMode = mode; }
        bool getDebugMode() const { return debugMode; }
    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

        btAlignedObjectArray<btCollisionShape*> collisionShapes;
        BulletDebugRenderer* bulletRenderer;

        bool debugMode = true;
};
