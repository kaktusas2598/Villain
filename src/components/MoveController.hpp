#ifndef __MOVE_CONTROLLER__
#define __MOVE_CONTROLLER__

#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class MoveController: public NodeComponent {
        public:
            DECLARE_COMPONENT_TYPE(MoveController);

            MoveController(float sp = 2.5f): speed(sp) {}

            virtual void handleInput(float deltaTime) override;
        private:
            float speed;
    };
}

#endif // __MOVE_CONTROLLER__
