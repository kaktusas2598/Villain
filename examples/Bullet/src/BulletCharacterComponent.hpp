#pragma once

#include "NodeComponent.hpp"
#include "BulletCharacterController.hpp"

class BulletCharacterComponent: public Villain::NodeComponent {
    public:
        BulletCharacterComponent(BulletCharacterController* control): controller(control) {}

        virtual void handleInput(float deltaTime) override;
    private:
        BulletCharacterController* controller;
};
