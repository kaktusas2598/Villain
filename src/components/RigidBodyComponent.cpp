#include "RigidBodyComponent.hpp"

#include "Engine.hpp"
#include "components/ScriptComponent.hpp"
#include "events/CollisionEvent.hpp"
#include "physics/TensorUtils.hpp"

namespace Villain {

    RigidBodyComponent::RigidBodyComponent(RigidBody* rigidBody, CollisionPrimitive* colShape)
        : body(rigidBody), collider(colShape) {
            VILLAIN_SET_COMPONENT_ID(RigidBodyComponent);
        }

    RigidBodyComponent::~RigidBodyComponent() {
        getParent()->getEngine()->getEventDispatcher()->unregisterCallback(BIND_EVENT_FN(onEvent));
        getParent()->getEngine()->getRigidBodyWorld()->removeBody(body);
        if (collider)
            getParent()->getEngine()->getRigidBodyWorld()->removeCollider(collider);
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

            // Only calculate inertia tensors for dynamic bodies
            if (body->getInverseMass() > 0.0f) {
                if (CollisionSphere* sphere = dynamic_cast<CollisionSphere*>(collider)) {
                    body->setInertiaTensor(TensorUtils::Sphere(sphere->radius, body->getMass()));
                } else if (CollisionBox* box = dynamic_cast<CollisionBox*>(collider)) {
                    body->setInertiaTensor(TensorUtils::Cuboid(box->halfSize * 2.0f, body->getMass()));
                } else {
                    VILLAIN_ERROR("Inertia tensor not supported for this collision primitive");
                }
            }

            getParent()->getEngine()->getEventDispatcher()->registerCallback(BIND_EVENT_FN(onEvent));
        }
    }

    void RigidBodyComponent::addForceGenerator(ForceGenerator* generator) {
        parent->getEngine()
            ->getRigidBodyWorld()->getForceRegistry().add(body, generator);
    }

    void RigidBodyComponent::onEvent(Event& event) {
        if (collider && event.getType() == EventType::CollisionEvent) {
            CollisionEvent& collisionEvent = static_cast<CollisionEvent&>(event);
            if (collisionEvent.getBody1() == body || collisionEvent.getBody2() == body) {
                ScriptComponent* scriptComponent = getParent()->getComponent<ScriptComponent>();
                if (scriptComponent) {
                    collisionEvent.setVerified(true);
                    // Make sure first body in event is same as one in node!
                    if (collisionEvent.getBody2() == body) {
                        collisionEvent.setBody2(collisionEvent.getBody1());
                        collisionEvent.setBody1(body);
                    }
                    scriptComponent->onEvent(collisionEvent);
                }
            }
        }
    }
}
