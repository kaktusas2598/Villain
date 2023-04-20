#pragma once

#include "NodeComponent.hpp"
#include "btBulletDynamicsCommon.h"

class BulletBodyComponent: public Villain::NodeComponent {
    public:
        BulletBodyComponent(btRigidBody* body): rigidBody(body) {}

        virtual void update(float deltaTime) override {
            // Synchonise node position with bullet rigid body's
            btVector3 origin = rigidBody->getWorldTransform().getOrigin();
            GetTransform()->setPos({origin.getX(), origin.getY(), origin.getZ()});
            // Synchonise node euler rotation with bullet rigid body's
            float x,y,z;
            rigidBody->getWorldTransform().getRotation().getEulerZYX(z, y, x);
            GetTransform()->setEulerRot(glm::degrees(x), glm::degrees(y), glm::degrees(z));
        }

        inline btRigidBody* getBody() const { return rigidBody; }
        bool Colliding = false;
    private:
        btRigidBody* rigidBody = nullptr;
};
