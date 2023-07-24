#pragma once

#include "NodeComponent.hpp"
#include "physics/NarrowPhase.hpp"
#include "physics/ForceGenerator.hpp"

namespace Villain {

    class RigidBodyComponent : public NodeComponent {
        public:
            RigidBodyComponent(RigidBody* rigidBody, CollisionPrimitive* colShape = nullptr);

            virtual void update(float deltaTime);
            virtual void addToEngine(Engine* engine);

            void addForceGenerator(ForceGenerator* generator);
        protected:
            RigidBody* body;
            CollisionPrimitive* collider;
    };
}
