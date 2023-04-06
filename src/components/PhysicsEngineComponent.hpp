#ifndef __PHYSICS_ENGINE_COMPONENT__
#define __PHYSICS_ENGINE_COMPONENT__

#include "../NodeComponent.hpp"
#include "../physics/PhysicsEngine.hpp"

namespace Villain {

    // NOTE: This class is temporary, don't like this solution
    class PhysicsEngineComponent : public NodeComponent {
        public:
            PhysicsEngineComponent(const PhysicsEngine& engine) :
                physicsEngine(engine) {}

            virtual void update(float deltaTime);

            inline const PhysicsEngine& getPhysicsEngine() const { return physicsEngine; }
        private:
            PhysicsEngine physicsEngine;
    };
}

#endif // __PHYSICS_ENGINE_COMPONENT__
