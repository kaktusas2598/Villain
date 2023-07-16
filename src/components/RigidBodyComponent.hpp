#pragma once

#include "NodeComponent.hpp"
#include "physics/ForceGenerator.hpp"

namespace Villain {

    class RigidBodyComponent : public NodeComponent {
        public:
            RigidBodyComponent(RigidBody* rigidBody);

            virtual void update(float deltaTime);
            virtual void addToEngine(Engine* engine);

            void addForceGenerator(ForceGenerator* generator);
        protected:
            RigidBody* body;
    };
}
