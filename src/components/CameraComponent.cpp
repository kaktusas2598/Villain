#include "CameraComponent.hpp"
#include "InputManager.hpp"

namespace Villain {

    void CameraComponent::handleInput(float deltaTime) {
        // TODO: Also would be useful to implement command pattern to abstract control keys and customize them
        if (camera->getProjectionType() == ProjectionType::THIRD_PERSON) {
            camera->setTarget(GetTransform()->getPos());
        } else {
            camera->setPosition(GetTransform()->getPos());
            camera->setRotation(GetTransform()->getEulerRot());
        }

        // TODO: probably not needed in case of 3rd person, or we just need to refactor setZoom method
        camera->setZoom(camera->getZoom() + InputManager::Instance()->mousewheel);
    }

    void CameraComponent::addToEngine(Engine* engine) {
        // NOTE: bad solution once we have multiple cameras in place
        engine->getRenderingEngine()->setMainCamera(*camera);
    }
}
