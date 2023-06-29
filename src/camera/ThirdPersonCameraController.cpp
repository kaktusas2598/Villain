#include "ThirdPersonCameraController.hpp"

namespace Villain {

    void ThirdPersonCameraController::update(float deltaTime) {
        // Implement the update logic for the first-person camera
        if (Engine::editModeActive()) return;

        // Not sure if really need this, it hides the cursor but there are other methods to do that
        SDL_SetRelativeMouseMode(SDL_TRUE);
        // Warping mouse cursor in center for the infinite camera movement
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), Engine::getScreenWidth()/2, Engine::getScreenHeight()/2);

        // Also disable mouse in edit mode for now
        glm::vec2 mouseOffsets = TheInputManager::Instance()->getMouseOffsets();

        // Mostly from ThinMatrix video, rotating around the target
        //// TODO: need to pass some additional args for this to enable pitch calc and rotation around player,
        //// like maybe we only want to rotate if RMB is pressed
        //pitch -= yOffset * mouseSensitivity;
        //// Calculate angle around target
        //angleAroundTarget -= xOffset * mouseSensitivity;
        //yaw = 180 - (target->getEulerRot().y + angleAroundTarget);
        ////yaw = 180 - angleAroundTarget;

        //float horizontalDistance = distanceToTarget * cos(glm::radians(pitch));
        //float verticalDistance = distanceToTarget * sin(glm::radians(pitch));

        ////float theta = target->getEulerRot().y + angleAroundTarget;
        //float theta = angleAroundTarget;
        //float offsetX = horizontalDistance * sin(glm::radians(theta));
        //float offsetZ = horizontalDistance * cos(glm::radians(theta));
        //position.x = target->getPos().x - offsetX;
        //position.y = target->getPos().y + verticalDistance;
        //position.z = target->getPos().z - offsetZ;

        ////target->setEulerRot(pitch, angleAroundTarget, 0.0f);


        mouseOffsets.x *= sensitivity;
        mouseOffsets.y *= sensitivity;

        float yaw = camera->getYaw();
        float pitch = camera->getPitch();
        yaw += mouseOffsets.x;
        pitch += mouseOffsets.y;

        // Prevent camera from flipping/ going underground
        if (pitch > 0.0f)
            pitch = 0.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        camera->setRotation({pitch, yaw, 0.0f});

        // Control distance to target with mouse wheel
        float mouseWheelOffset = InputManager::Instance()->mousewheel;
        float newDistanceToTarget = camera->getDistanceToTarget() - mouseWheelOffset;
        camera->setDistanceToTarget(newDistanceToTarget);
        if (newDistanceToTarget < 1.0f)
            camera->setDistanceToTarget(1.0f);


        // TODO: movement logic
        camera->setTarget(camera->GetTransform());
    }
}
