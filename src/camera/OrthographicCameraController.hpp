#pragma once

#include "CameraController.hpp"
#include "Engine.hpp"

namespace Villain {

    class OrthographicCameraController : public CameraController {
        public:
            // NOTE: Recommended zNear/zFar - -1000/1000
            OrthographicCameraController(Camera* cam): CameraController(cam) {
                cam->setRotation({45.0f, 45.0f, 0.0f});
                cam->setMovementSpeed(2.5f);
                cam->setSensitivity(sensitivity);
                cam->setZoom(1.0f);
            }

            void update(float deltaTime) override;

        private:
            bool moveControls = true;
            float sensitivity = 0.1f;
            float movementSpeed = 1.0f;
    };

}
