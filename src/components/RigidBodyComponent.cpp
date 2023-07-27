#include "RigidBodyComponent.hpp"

#include "Engine.hpp"
#include "physics/TensorUtils.hpp"

namespace Villain {

    RigidBodyComponent::RigidBodyComponent(RigidBody* rigidBody, CollisionPrimitive* colShape)
        : body(rigidBody), collider(colShape) {
            VILLAIN_SET_COMPONENT_ID(RigidBodyComponent);
        }

    void RigidBodyComponent::update(float deltaTime) {
        // Set position of scene graph node based on RigidBody position
        GetTransform()->setPos(body->getPosition());
        // Set rotation of scene graph node based on RigidBody orientation
        GetTransform()->setRot(body->getOrientation());
    }

    void RigidBodyComponent::addToEngine(Engine* engine) {
        // Rigid body will be automatically added to physics world once it has a parent
        engine->getRigidBodyWorld()->getBodies().push_back(body);

        if (collider) {
            engine->getRigidBodyWorld()->getColliders().push_back(collider);

            if (CollisionSphere* sphere = dynamic_cast<CollisionSphere*>(collider)) {
                body->setInertiaTensor(TensorUtils::Sphere(sphere->radius, body->getMass()));
            } else if (CollisionBox* box = dynamic_cast<CollisionBox*>(collider)) {
                body->setInertiaTensor(TensorUtils::Cuboid(box->halfSize * 2.0f, body->getMass()));
            } else {
                VILLAIN_ERROR("Inertia tensor not supported for this collision primitive");
            }
        }
    }

    void RigidBodyComponent::addForceGenerator(ForceGenerator* generator) {
                parent->getEngine()
                    ->getRigidBodyWorld()->getForceRegistry().add(body, generator);
            }

}
