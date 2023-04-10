#pragma once

#include "components/MeshRenderer.hpp"
#include "Timer.hpp"

class Monster : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Monster();
        virtual void update(float deltaTime) override;
};
