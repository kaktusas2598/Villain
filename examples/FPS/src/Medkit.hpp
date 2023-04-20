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
        Level* currentLevel = nullptr;
};
