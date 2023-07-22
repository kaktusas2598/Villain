#pragma once

#include "Engine.hpp"
#include "CameraController.hpp"

namespace Villain {

    class FirstPersonCameraController : public CameraController {
        public:
            FirstPersonCameraController(Camera* cam): CameraController(cam) {
                cam->setRotation({0.0f, -90.0f, 0.0f});
                cam->setMovementSpeed(2.5f);
                cam->setSensitivity(sensitivity);
                cam->setZoom(45.0f);
            }

            void update(float deltaTime) override;

        private:
            float sensitivity = 0.1f;
            bool constrainPitch = true;

    };

}
