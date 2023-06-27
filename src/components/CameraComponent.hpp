#ifndef __CAMERA_COMPONENT__
#define __CAMERA_COMPONENT__

#include "../Camera.hpp"
#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class CameraComponent: public NodeComponent {
        public:
            CameraComponent(Camera* cam): camera(cam) {
                // HACK: Generating unique id for component mask, this is not great though, because it
                // will really complicate writing new components and having to add this every time!
                id = GetId<CameraComponent>();
            }

            Camera* getCamera() const { return camera; }

            virtual void handleInput(float deltaTime) override;
            virtual void addToEngine(Engine* engine) override;
        private:
            Camera* camera = nullptr;
    };
}

#endif // __CAMERA_COMPONENT__
