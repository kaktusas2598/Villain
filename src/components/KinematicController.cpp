#include "KinematicController.hpp"

#include "Input.hpp"

namespace Villain {

    void KinematicController::addToEngine(Engine* engine) {
        RigidBodyComponent::addToEngine(engine);
    }

    void KinematicController::update(float deltaTime) {
        RigidBodyComponent::update(deltaTime);
    }

    void KinematicController::handleInput(float deltaTime) {
        if (Input::Get()->isKeyDown(SDLK_w)) {
            body->addForce(GetTransform()->getForward() * body->getMass());
        }
        if (Input::Get()->isKeyDown(SDLK_s)) {
            body->addForce(GetTransform()->getForward() * -body->getMass());
        }
        if (Input::Get()->isKeyDown(SDLK_a)) {
            body->addForce(GetTransform()->getRight() * -body->getMass());
        }
        if (Input::Get()->isKeyDown(SDLK_d)) {
            body->addForce(GetTransform()->getRight() * body->getMass());
        }
        if (Input::Get()->isKeyDown(SDLK_SPACE)) {
            body->addForce(GetTransform()->getUp() * body->getMass());
        }
        if (Input::Get()->isKeyDown(SDLK_LSHIFT)) {
            body->addForce(GetTransform()->getUp() * -body->getMass());
        }
    }
}
