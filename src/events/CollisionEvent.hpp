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

            void setBody1(RigidBody* rb1) { body1 = rb1; }
            void setBody2(RigidBody* rb2) { body2 = rb2; }

            void setVerified(bool verify) { verified = verify; }
            bool isVerified() const { return verified; }

            VILLAIN_EVENT_TYPE(CollisionEvent);

        private:
            RigidBody* body1;
            RigidBody* body2; // TODO: or do we want to use CollisionPrimitive or sth else here?
            // TODO: maybe contact data gets dispatched as well
            bool isTrigger; // TODO: trigger wil not generate response force

            bool verified = false; ///< Event will be verified only if it's being forwarded to the same node in the scene graph
    };

};
