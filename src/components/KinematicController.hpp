#pragma once

#include "RigidBodyComponent.hpp"

namespace Villain {

    class KinematicController : public RigidBodyComponent {
        public:
            KinematicController(RigidBody* planeBody, CollisionPrimitive* colShape = nullptr)
                : RigidBodyComponent(planeBody, colShape) {}

            virtual void addToEngine(Engine* engine);
            virtual void update(float deltaTime);
            virtual void handleInput(float deltaTime);
    };
}
