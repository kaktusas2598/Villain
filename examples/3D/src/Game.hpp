#pragma once

#include "Application.hpp"
#include "camera/Camera.hpp"
#include "components/FlightController.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/Model.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void handleEvents(float deltaTime);
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppPostUpdate(float dt);
        void onAppRender(float deltaTime);

    private:
        Villain::DebugRenderer debugRenderer;
        Villain::Particle* playerBody = nullptr;
        Villain::FlightController* aircraft = nullptr;
};
