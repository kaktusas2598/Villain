#pragma once

#include "EventDispatcher.hpp"
#include "physics/RigidBody.hpp"

namespace Villain {

    /// Dispatched upon 2 Rigid bodies colliding
    class CollisionEvent : public Event {
        public:
            CollisionEvent(RigidBody* a, RigidBody* b): body1(a), body2(b) {}

            RigidBody* getBody1() const { return body1; }
            RigidBody* getBody2() const { return body2; }

            VILLAIN_EVENT_TYPE(CollisionEvent);

        private:
            RigidBody* body1;
            RigidBody* body2; // TODO: or do we want to use CollisionPrimitive or sth else here?
            // TODO: maybe contact data gets dispatched as well
            bool isTrigger; // TODO: trigger wil not generate response force
    };

};
