#pragma once

#include "NodeComponent.hpp"
#include "btBulletDynamicsCommon.h"

class BulletBodyComponent: public Villain::NodeComponent {
    public:
        BulletBodyComponent(btRigidBody* body): rigidBody(body) {}

        virtual void update(float deltaTime) override {
            btVector3 origin = rigidBody->getWorldTransform().getOrigin();
            GetTransform()->setPos({origin.getX(), origin.getY(), origin.getZ()});

            float x,y,z;
            rigidBody->getWorldTransform().getRotation().getEulerZYX(x, y, z);
            //printf("Bullet body example: %fX, %fY, %fZ\n", x, y, z);
            GetTransform()->setEulerRot(glm::degrees(x), glm::degrees(y), glm::degrees(z));
        }
    private:
        btRigidBody* rigidBody = nullptr;
};
