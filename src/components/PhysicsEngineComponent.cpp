#include "PhysicsEngineComponent.hpp"

namespace Villain {

    void PhysicsEngineComponent::update(float deltaTime) {
        physicsEngine.simulate(deltaTime);
    }
}
