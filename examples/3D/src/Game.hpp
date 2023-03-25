#ifndef __GAME__
#define __GAME__

#include "Engine.hpp"

#include "Model.hpp"
#include "Camera.hpp"

#include "DebugRenderer.hpp"

class Game : public Villain::Engine {
    public:
        Game();
        ~Game();

        void handleEvents(float deltaTime);
        void addStates() {}
        void onInit() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppPostUpdate(float dt);
        void onAppRender(float deltaTime);
        void onAppWindowResize(int newWidth, int newHeight);

    private:
        Villain::Camera camera;
        Villain::Model* model3D = nullptr;

        Villain::DebugRenderer debugRenderer;
};

#endif // __GAME__
