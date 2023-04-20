#pragma once

#include "btBulletDynamicsCommon.h"
// Custom renderer
#include "BulletDebugRenderer.hpp"
// For soft bodies
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
// Ray cast callbacks
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

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

        //btDynamicsWorld* getWorld() const { return dynamicsWorld; }
        btSoftRigidDynamicsWorld* getWorld() const { return dynamicsWorld; }

        void addAction(btActionInterface* action) {
            dynamicsWorld->addAction(action);
        }

        inline void setDebugMode(int mode) {
            dynamicsWorld->getDebugDrawer()->setDebugMode(mode);
        }
    private:
        btSoftBodyRigidBodyCollisionConfiguration* collisionConfiguration;
        //btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        //btDiscreteDynamicsWorld* dynamicsWorld;
        btSoftRigidDynamicsWorld* dynamicsWorld;
        btSoftBodySolver* softBodySolver;

        btAlignedObjectArray<btCollisionShape*> collisionShapes;
        BulletDebugRenderer* bulletRenderer;
};
