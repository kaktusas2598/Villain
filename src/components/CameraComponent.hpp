#ifndef __CAMERA_COMPONENT__
#define __CAMERA_COMPONENT__

#include "../Camera.hpp"
#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class CameraComponent: public NodeComponent {
        public:
            CameraComponent(Camera* cam): camera(cam) {}

            virtual void handleInput(float deltaTime) override;
            virtual void addToEngine(Engine* engine) override;
        private:
            Camera* camera = nullptr;
    };
}

#endif // __CAMERA_COMPONENT__
