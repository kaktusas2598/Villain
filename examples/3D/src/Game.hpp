#ifndef __GAME__
#define __GAME__

#include "Application.hpp"

#include "Model.hpp"
#include "SkyBox.hpp"
#include "Camera3D.hpp"

#include "DebugRenderer.hpp"

class Game : public Villain::Application {
    public:
        ~Game();

        void init();
        void handleEvents(float deltaTime);
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppPostUpdate(float dt);
        void onAppRender(float deltaTime);
        void onAppWindowResize(int newWidth, int newHeight);

    private:
        Villain::Camera3D camera;
        Villain::Model* model3D = nullptr;
        std::unique_ptr<Villain::SkyBox> skybox;

        Villain::DebugRenderer debugRenderer;

        Villain::SceneNode* planeNode;
};

#endif // __GAME__
