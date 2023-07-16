#pragma once

#include "RigidBodyComponent.hpp"

namespace Villain {

    class KinematicController : public RigidBodyComponent {
        public:
            KinematicController(RigidBody* planeBody) : RigidBodyComponent(planeBody) {}

            virtual void addToEngine(Engine* engine);
            virtual void update(float deltaTime);
            virtual void handleInput(float deltaTime);
    };
}
