#include "PhysicsObjectComponent.hpp"

namespace Villain {

    void PhysicsObjectComponent::update(float deltaTime) {
        GetTransform()->setPos(physicsObject->getPosition());
    }
}
