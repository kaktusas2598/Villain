#pragma once

#include "components/MeshRenderer.hpp"
#include "Timer.hpp"

#include "Level.hpp"

enum class AIState { STATE_IDLE = 0, STATE_CHASE, STATE_ATTACK, STATE_DYING, STATE_DEAD };

class Monster : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Monster(Level* level);
        virtual void update(float deltaTime) override;

        glm::vec2 getSize();
        void damage(int amount);
        int getDamage() const { return 15.0f; }
    private:
        void idleUpdate(float deltaTime);
        void chaseUpdate(float deltaTime);
        void attackUpdate(float deltaTime);
        void dyingUpdate(float deltaTime);
        void deadUpdate(float deltaTime);

        AIState currentState = AIState::STATE_IDLE;
        float distance; // Distance to Player/Camera
        glm::vec3 orientation; // Orientation towards camera
        Level* currentLevel = nullptr;
        bool canLook = false;
        bool canAttack = false;
        Villain::Timer timer;
        int health = 100;
};
