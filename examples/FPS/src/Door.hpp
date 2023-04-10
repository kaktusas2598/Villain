#pragma once

#include "components/MeshRenderer.hpp"
#include "Timer.hpp"

class Door : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Door(const glm::vec3& open);
        void open();
        void setOpenPos(const glm::vec3& openPos) { openPosition = openPos; closePosition = GetTransform()->getPos(); }
        glm::vec2 getSize();
        //virtual void handleInput(float deltaTime) override;
        virtual void update(float deltaTime) override;
        const float Length = 1.0f;
        const float Height = 2.0f;
        const float Width = 0.125f;
        const float Start = 0.0f; // Just for nicer formatting
    private:
        bool opening = false;
        // Animation keyframes
        Villain::Timer animTimer;
        double openingStartTime;
        double openTime;
        double closingStartTime;
        double closeTime;

        glm::vec3 openPosition;
        glm::vec3 closePosition;
};
