#pragma once

#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    // Can be attached to any Graph node in scene to control it via keyboard
    class MoveController: public NodeComponent {
        public:
            MoveController(float sp = 2.5f): speed(sp) {}

            virtual void handleInput(float deltaTime) override;
        private:
            float speed;
    };
}
