#include "RigidBodyComponent.hpp"

#include "Engine.hpp"

namespace Villain {

    RigidBodyComponent::RigidBodyComponent(RigidBody* rigidBody) : body(rigidBody) {}

    void RigidBodyComponent::update(float deltaTime) {
        // Set position of scene graph node based on RigidBody position
        GetTransform()->setPos(body->getPosition());
        // Set rotation of scene graph node based on RigidBody orientation
        GetTransform()->setRot(body->getOrientation());
    }

    void RigidBodyComponent::addToEngine(Engine* engine) {
        // Rigid body will be automatically added to physics world once it has a parent
        engine->getRigidBodyWorld()->getBodies().push_back(body);
    }
}
