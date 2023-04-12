#pragma once

#include "Application.hpp"
#include "Camera3D.hpp"
#include "Level.hpp"

#include "Camera2D.hpp"
#include "FreeType.hpp"
#include "SpriteBatch.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void addStates() {}
        void onAppPreUpdate(float dt);
        void onAppRender(float dt);
    private:
        Villain::Camera3D* camera;
        Level* currentLevel;

        // For static stuff, player gun, HUD, text, should be part of scene graph
        Villain::SpriteBatch spriteBatch;
        Villain::Camera2D hudCamera;
        Villain::Shader* hudShader = nullptr;
        Villain::Shader* textShader = nullptr;
        Villain::FreeType* freeType;
};
