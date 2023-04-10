#pragma once

#include "NodeComponent.hpp"
#include "Level.hpp"

class Player : public Villain::NodeComponent {
    public:
        Player(Level* level, float sp = 3.f): currentLevel(level), speed(sp) {}

        virtual void handleInput(float deltaTime) override;
        virtual void update(float deltaTime) override;
    private:
        Level* currentLevel = nullptr;
        float speed;
        glm::vec3 movement = glm::vec3(0.f);
};
