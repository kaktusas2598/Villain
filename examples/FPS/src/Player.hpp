#pragma once

#include "NodeComponent.hpp"

class Player : public Villain::NodeComponent {
    public:
        Player() {}

        virtual void handleInput(float deltaTime) override;
        virtual void update(float deltaTime) override;
    private:
};
