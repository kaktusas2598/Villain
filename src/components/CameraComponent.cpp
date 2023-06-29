#include "CameraComponent.hpp"
#include "InputManager.hpp"

namespace Villain {

    void CameraComponent::handleInput(float deltaTime) {
        if (camera->getProjectionType() == ProjectionType::THIRD_PERSON) {
            camera->setTarget(GetTransform());
            // TODO: rotation would be different if we want to rotate AROUND the target
            camera->setRotation(GetTransform()->getEulerRot());
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
