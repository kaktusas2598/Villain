#pragma once

#include "RigidBodyComponent.hpp"

namespace Villain {

    /// Attach to node to create kinematic rigid body which can be controlled via input
    class KinematicController : public RigidBodyComponent {
        public:
            KinematicController(RigidBody* planeBody, CollisionPrimitive* colShape = nullptr);

            virtual void addToEngine(Engine* engine);
            virtual void update(float deltaTime);
            virtual void handleInput(float deltaTime);
    };
}
