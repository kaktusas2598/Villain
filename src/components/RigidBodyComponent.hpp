#pragma once

#include "NodeComponent.hpp"
#include "physics/RigidBody.hpp"

namespace Villain {

    class RigidBodyComponent : public NodeComponent {
        public:
            RigidBodyComponent(RigidBody* rigidBody);

            virtual void update(float deltaTime);
            virtual void addToEngine(Engine* engine);
        private:
            RigidBody* body;
    };
}
