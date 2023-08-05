#pragma once

#include "EventDispatcher.hpp"
#include "NodeComponent.hpp"
#include "physics/NarrowPhase.hpp"
#include "physics/ForceGenerator.hpp"

namespace Villain {

    /// Represents a rigid body in screne graph, can associate force generators and optionally specify collider
    class RigidBodyComponent : public NodeComponent {
        public:
            RigidBodyComponent(RigidBody* rigidBody, CollisionPrimitive* colShape = nullptr);
            ~RigidBodyComponent();

            virtual void update(float deltaTime);
            virtual void addToEngine(Engine* engine);

            void addForceGenerator(ForceGenerator* generator);

            void onEvent(Event& event);

            RigidBody* getBody() const { return body; }
            CollisionPrimitive* getCollider() const { return collider; }
        protected:
            RigidBody* body;
            CollisionPrimitive* collider;
    };
}
