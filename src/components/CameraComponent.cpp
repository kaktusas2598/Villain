#include "CameraComponent.hpp"

namespace Villain {

    void CameraComponent::handleInput(float deltaTime) {
        // TODO: Also would be useful to implement command pattern to abstract control keys and customize them
        camera->setPosition(GetTransform()->getPos());
        camera->setRotation(GetTransform()->getEulerRot());
    }

    void CameraComponent::addToEngine(Engine* engine) {
        // NOTE: bad solution once we have multiple cameras in place
        engine->getRenderingEngine()->setMainCamera(*camera);
    }
}
