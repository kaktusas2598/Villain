#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "camera/Camera.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/Model.hpp"

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
        Villain::DebugRenderer debugRenderer;
};

#endif // __GAME__
