#pragma once

#include "Application.hpp"
#include "camera/Camera.hpp"
#include "rendering/DebugRenderer.hpp"
#include "terrain/FaultFormationTerrain.hpp"
#include "terrain/MidpointDisplacementTerrain.hpp"
#include "terrain/Terrain.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppRender(float deltaTime);
        void onAppImGuiRender(float deltaTime);

    private:
        Villain::Camera* camera;
        Villain::SceneNode* cameraNode;
        Villain::DebugRenderer debugRenderer;
        Villain::SceneNode* lightNode;

        Villain::Terrain baseTerrain;
        Villain::FaultFormationTerrain faultFormationTerrain;
        Villain::MidpointDisplacementTerrain midpointDisplacementTerrain;

        Villain::Texture* terrainGeneratedTexture = nullptr;
        Villain::Texture* heightMapDebugTexture = nullptr;

        int terrainType = 2;
};
