#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera.hpp"
#include "rendering/DebugRenderer.hpp"

class Game : public Villain::Application {
    public:
        void init();
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppRender(float deltaTime);

    private:
        Villain::Camera* camera;
        Villain::DebugRenderer debugRenderer;
};

#endif // __GAME__
