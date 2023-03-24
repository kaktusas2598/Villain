#ifndef __GAME__
#define __GAME__

#include "DebugRenderer.hpp"
#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Timer.hpp"

#include "Model.hpp"
#include "Camera.hpp"

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
        Camera camera;
        Villain::Model* model3D;

        Villain::DebugRenderer debugRenderer;
};

#endif // __GAME__
