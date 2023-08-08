#pragma once

#include "CameraController.hpp"

#include "Engine.hpp"

namespace Villain {

    class ThirdPersonCameraController : public CameraController {
        public:
            ThirdPersonCameraController(Camera* cam): CameraController(cam) {
                cam->setRotation({20.0f, 0.0f, 0.0f});
                cam->setMovementSpeed(2.5f);
                cam->setSensitivity(sensitivity);
                cam->setZoom(45.0f);
                camera->setDistanceToTarget(defaultDistance);
            }

            void update(float deltaTime) override;

        private:
            void updateFollowMode(float deltaTime);
            void updateRotateMode(float deltaTime);

            enum class CameraMode {
                FOLLOW_MODE,
                ROTATE_MODE
            };

            CameraMode mode = CameraMode::FOLLOW_MODE;
            float sensitivity = 0.1f;
            float defaultDistance = 50.0f;
    };

}
