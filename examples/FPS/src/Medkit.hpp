#pragma once

#include "components/MeshRenderer.hpp"
#include "Timer.hpp"

#include "Level.hpp"

class Medkit : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Medkit(Level* level);
        virtual void update(float deltaTime) override;

        glm::vec2 getSize();
        void damage(int amount);
        int getDamage() const { return 20.0f; }
    private:
        float distance; // Distance to Player/Camera
        glm::vec3 orientation; // Orientation towards camera
        Level* currentLevel = nullptr;
        bool canLook = false;
        bool canAttack = false;
        Villain::Timer timer;
        int health = 100;
        float deathTime = 0.0f;
};
