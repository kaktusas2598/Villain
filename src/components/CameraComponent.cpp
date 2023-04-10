#include "CameraComponent.hpp"

#include "../Camera2D.hpp"
#include "../Camera3D.hpp"

namespace Villain {

    void CameraComponent::handleInput(float deltaTime) {
        // TODO: Also would be useful to implement command pattern to abstract control keys and customize them
        Camera3D* cam3D = dynamic_cast<Camera3D*>(camera);
        if (cam3D != nullptr) {
            cam3D->setPosition(GetTransform()->getPos());
            cam3D->setRotation(GetTransform()->getEulerRot());
        }
    }

    void CameraComponent::addToEngine(Engine* engine) {
        // NOTE: bad solution once we have multiple cameras in place
        engine->getRenderingEngine()->setMainCamera(*camera);
    }
}
