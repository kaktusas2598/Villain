#pragma once

#include "components/MeshRenderer.hpp"
#include "Timer.hpp"

enum class AIState { STATE_IDLE = 0, STATE_CHASE, STATE_ATTACK, STATE_DYING, STATE_DEAD };

class Monster : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Monster();
        virtual void update(float deltaTime) override;

    private:
        void idleUpdate();
        void chaseUpdate();
        void attackUpdate();
        void dyingUpdate();
        void deadUpdate();

        AIState currentState = AIState::STATE_IDLE;
};
