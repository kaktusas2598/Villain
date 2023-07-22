#pragma once

#include "../NodeComponent.hpp"
#include "../physics/PhysicsObject.hpp"

namespace Villain {

    // NOTE: This class might be temporary?
    class PhysicsObjectComponent : public NodeComponent {
        public:
            PhysicsObjectComponent(const PhysicsObject* object) :
                physicsObject(object) {}

            virtual void update(float deltaTime);
        private:
            const PhysicsObject* physicsObject;
    };
}
