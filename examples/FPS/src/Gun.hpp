#pragma once

#include "components/MeshRenderer.hpp"

// FIXME: THis class is not really being utilised properly, to make gun always draw in front of a Player I had to
// use Villain::SpriteBatch whicch is not great as I wanted to use Scene Graph for everything, but I will keep this in here
// for now in case I fix it later on
class Gun : public Villain::MeshRenderer<VertexP1N1UV> {
    public:
        Gun();
        virtual void update(float deltaTime) override;
};
