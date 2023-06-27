#ifndef __LOOK_CONTROLLER__
#define __LOOK_CONTROLLER__

#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class LookController: public NodeComponent {
        public:
            DECLARE_COMPONENT_TYPE(LookController);

            LookController(float sens = 0.1f, bool constrainP = true) :
                sensitivity(sens), constrainPitch(constrainP) {}

            virtual void handleInput(float deltaTime) override;
        private:
            float sensitivity;
            bool constrainPitch;
    };
}

#endif // __LOOK_CONTROLLER__
