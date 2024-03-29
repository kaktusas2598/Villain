#pragma once

#include "NodeComponent.hpp"
#include "Level.hpp"

class Player : public Villain::NodeComponent {
    public:
        Player(Level* level, float sp = 3.f): currentLevel(level), speed(sp), health(100) {}

        virtual void handleInput(float deltaTime) override;
        virtual void update(float deltaTime) override;

        int getDamage() const { return 20.0f; }
        int getMaxHealth() const { return 100.0f; }
        int getHealth() const { return health; }
        void damage(int amount);

        static const float PLAYER_SIZE;
    private:
        Level* currentLevel = nullptr;
        float speed;
        glm::vec3 movement = glm::vec3(0.f);
        int health = 100;
};
