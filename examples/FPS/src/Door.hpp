#pragma once

#include "components/MeshRenderer.hpp"

class Door : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Door();
        //virtual void handleInput(float deltaTime) override;
        //virtual void update(float deltaTime) override;
        const float Length = 1.0f;
        const float Height = 2.0f;
        const float Width = 0.125f;
        const float Start = 0.0f; // Just for nicer formatting
    private:
};
