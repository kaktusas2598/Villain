#pragma once

#include "Application.hpp"
#include "camera/Camera.hpp"
#include "audio/AudioSource.hpp"
#include "components/FlightController.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/Model.hpp"
#include "rendering/SkyDome.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void handleEvents(float deltaTime);
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppPostUpdate(float dt);
        void onAppRender(float deltaTime);

        void onEvent(Villain::Event& event);

        void rayCallback(Villain::RayHitResult& result);
    private:
        Villain::DebugRenderer debugRenderer;
        //Villain::Particle* playerBody = nullptr;
        Villain::RigidBody* playerBody = nullptr;
        Villain::FlightController* aircraft = nullptr;

        Villain::CollisionSphere* one = nullptr;
        Villain::CollisionSphere* two = nullptr;
        Villain::CollisionBox* three = nullptr;
        Villain::CollisionBox* box = nullptr;
        Villain::RigidBody* sphere1 = nullptr;
        Villain::RigidBody* sphere2 = nullptr;

        Villain::RigidBody* boxBody = nullptr;

        Villain::AudioSource* zombieMono;

        Villain::SkyDome* skydomeTest;
};
