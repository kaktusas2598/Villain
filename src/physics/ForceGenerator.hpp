#pragma once

#include "RigidBody.hpp"

namespace Villain {

    // Can be used to add a force to one or more rigid bodies
    class ForceGenerator {
        public:
            virtual void updateForce(RigidBody* body, float deltaTime) = 0;
    };
}
