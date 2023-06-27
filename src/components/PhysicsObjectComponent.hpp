#ifndef __PHYSICS_OBJECT_COMPONENT__
#define __PHYSICS_OBJECT_COMPONENT__

#include "../NodeComponent.hpp"
#include "../physics/PhysicsObject.hpp"

namespace Villain {

    // NOTE: This class might be temporary?
    class PhysicsObjectComponent : public NodeComponent {
        public:
            DECLARE_COMPONENT_TYPE(PhysicsObjectComponent);

            PhysicsObjectComponent(const PhysicsObject* object) :
                physicsObject(object) {}

            virtual void update(float deltaTime);
        private:
            const PhysicsObject* physicsObject;
    };
}

#endif // __PHYSICS_OBJECT_COMPONENT__
