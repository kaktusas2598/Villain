#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera3D.hpp"
#include "rendering/DebugRenderer.hpp"

#include "BulletEngine.hpp"

class Game : public Villain::Application {
    public:
        ~Game();
        void init();
        void handleEvents(float deltaTime);
        void addStates() {}

        // Villain Engine callbacks
        void onAppPreUpdate(float dt);
        void onAppRender(float deltaTime);

    private:
        BulletEngine* physicsEngine = nullptr;
        Villain::Camera3D camera;
        Villain::DebugRenderer debugRenderer;

        void createGround();
        void createRigidBody();

        void addPlayer();
        void addRigidBoxes();
};

#endif // __GAME__
