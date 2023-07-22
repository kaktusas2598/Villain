#pragma once

#include "../Engine.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    class LookController: public NodeComponent {
        public:
            LookController(float sens = 0.1f, bool constrainP = true) :
                sensitivity(sens), constrainPitch(constrainP) {}

            virtual void handleInput(float deltaTime) override;
        private:
            float sensitivity;
            bool constrainPitch;
    };
}
