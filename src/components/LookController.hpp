#pragma once

#include "Engine.hpp"
#include "NodeComponent.hpp"

namespace Villain {

    class LookController: public NodeComponent {
        public:
            LookController(float sens = 0.1f, bool constrainP = true) :
                sensitivity(sens), constrainPitch(constrainP) {
                    VILLAIN_SET_COMPONENT_ID(LookController);
                }

            virtual void handleInput(float deltaTime) override;

            bool* getConstrainPitch() { return &constrainPitch; }
            float* getSensitivity() { return &sensitivity; }

        private:
            float sensitivity;
            bool constrainPitch;
    };
}
