#pragma once

#include "Application.hpp"
#include "Camera3D.hpp"
#include "Level.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void addStates() {}
        void onAppPreUpdate(float dt);
    private:
        Villain::Camera3D* camera;
        Level* currentLevel;
};
