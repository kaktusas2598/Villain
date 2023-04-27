#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/SkyBox.hpp"

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
        Villain::Camera* camera;
        std::unique_ptr<Villain::SkyBox> skybox;
        Villain::DebugRenderer debugRenderer;

        // TEMP: For debugging shadow mapping for directional lights
        Villain::SceneNode* directionalLight;
};

#endif // __GAME__
