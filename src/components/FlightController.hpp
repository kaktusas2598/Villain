#pragma once

#include "RigidBodyComponent.hpp"
#include "physics/generators/force/AeroControl.hpp"

namespace Villain {

    // FIXME: Applying any throttle accelerates aircraft way too quickly, and this only happens if any of
    // Aero/AeroControl generators are attached indicating either problem with them or possibly tensors used for them
    // or for the plane
    class FlightController : public RigidBodyComponent {
        public:
            FlightController(RigidBody* planeBody, glm::vec3* windspeed)
                : RigidBodyComponent(planeBody), windSpeed(windspeed) {}

            virtual void addToEngine(Engine* engine);
            virtual void update(float deltaTime);
            virtual void handleInput(float deltaTime);

            void resetPlane();

        private:
            AeroControl* leftWing = nullptr;
            AeroControl* rightWing = nullptr;
            AeroControl* rudder = nullptr;
            Aero* tail = nullptr; //< Not controllable ATM

            float leftWingControl = 0.0f;
            float rightWingControl = 0.0f;
            float rudderControl = 0.0f;
            float throttle = 0.0f;

            glm::vec3* windSpeed;
    };
}
