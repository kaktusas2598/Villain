#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera.hpp"
#include "FaultFormationTerrain.hpp"
#include "MidpointDisplacementTerrain.hpp"
#include "Terrain.hpp"
#include "rendering/DebugRenderer.hpp"

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
        Villain::DebugRenderer debugRenderer;
        Villain::SceneNode* lightNode;

        Terrain baseTerrain;
        FaultFormationTerrain faultFormationTerrain;
        MidpointDisplacementTerrain midpointDisplacementTerrain;

        Villain::Texture* terrainGeneratedTexture = nullptr;
        Villain::Texture* heightMapDebugTexture = nullptr;

        int terrainType = 2;
};

#endif // __GAME__
