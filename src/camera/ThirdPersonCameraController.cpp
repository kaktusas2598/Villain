#include "ThirdPersonCameraController.hpp"

namespace Villain {

    void ThirdPersonCameraController::update(float deltaTime) {
        // In engine editor overlay, disable camera rotations
        if (Engine::editModeActive()) return;

        // Not sure if really need this, it hides the cursor but there are other methods to do that
        SDL_SetRelativeMouseMode(SDL_TRUE);
        // Warping mouse cursor in center for the infinite camera movement
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), Engine::getScreenWidth()/2, Engine::getScreenHeight()/2);


        // If RMB is held, rotate around target, else follow the target
        if (InputManager::Instance()->isKeyDown(SDL_BUTTON_RIGHT)) {
            mode = CameraMode::ROTATE_MODE;
        } else {
            mode = CameraMode::FOLLOW_MODE;
        }

        switch (mode) {
            case CameraMode::FOLLOW_MODE:
                updateFollowMode(deltaTime);
                break;
            case CameraMode::ROTATE_MODE:
                updateRotateMode(deltaTime);
                break;
        }
    }

    void ThirdPersonCameraController::updateFollowMode(float deltaTime) {
        glm::vec2 mouseOffsets = InputManager::Instance()->getMouseOffsets();
        mouseOffsets *= sensitivity;

        float yaw = camera->getYaw();
        float pitch = camera->getPitch();
        yaw += mouseOffsets.x;
        pitch += mouseOffsets.y;

        // Prevent camera from flipping/ going underground
        if (pitch > 0.0f)
            pitch = 0.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // If camera belongs to a node, also set node rotation and invert yaw, so camera is looking at parent's back
        camera->setRotation({pitch, yaw, 0.0f});
        if (camera->getParent())
            camera->GetTransform()->setEulerRot(0.0, yaw, 0.0);
        // FIXME: to make model renderer rotat in same direction, need to set -yaw, but when model walks in opposide direction
        // because of wrong front/right vectors probably, but we could fix this
            //camera->GetTransform()->setEulerRot(0.0, -yaw, 0.0);


        // Control distance to target with mouse wheel
        float mouseWheelOffset = InputManager::Instance()->mousewheel;
        float newDistanceToTarget = camera->getDistanceToTarget() - mouseWheelOffset;
        camera->setDistanceToTarget(newDistanceToTarget);
        if (newDistanceToTarget < 1.0f)
            camera->setDistanceToTarget(1.0f);


        // TODO: movement logic WASD
        camera->setTarget(camera->GetTransform());
    }

    void ThirdPersonCameraController::updateRotateMode(float deltaTime) {
        camera->setTarget(camera->GetTransform());

        glm::vec3 cameraPos = camera->getPosition();
        glm::vec3 targetPos = camera->GetTransform()->getPos();

        glm::vec2 mouseOffsets = InputManager::Instance()->getMouseOffsets();
        mouseOffsets *= sensitivity;

        float angleAroundTarget = camera->getYaw() + mouseOffsets.x;
        // For not pitch is static
        //pitch -= yOffset * mouseSensitivity;

        // Calculate new camera positions based on rotation around the target
        float distanceToTarget = camera->getDistanceToTarget();
        float offsetX = distanceToTarget * sin(glm::radians(angleAroundTarget));
        float offsetZ = distanceToTarget * cos(glm::radians(angleAroundTarget));
        glm::vec3 newCameraPos = targetPos + glm::vec3(offsetX, 0.0f, offsetZ);

        camera->setPosition(newCameraPos);
        // Only set camera's rotation, not parent's node to make it look around
        camera->setRotation({camera->getPitch(), angleAroundTarget, 0.0f});

        // Control distance to target with mouse wheel
        float mouseWheelOffset = InputManager::Instance()->mousewheel;
        float newDistanceToTarget = camera->getDistanceToTarget() - mouseWheelOffset;
        camera->setDistanceToTarget(newDistanceToTarget);
        if (newDistanceToTarget < 1.0f)
            camera->setDistanceToTarget(1.0f);

    }
}
