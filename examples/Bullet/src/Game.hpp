#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "camera/Camera.hpp"
#include "rendering/DebugRenderer.hpp"
#include "rendering/SkyBox.hpp"

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

        // Static so that scene graph parent of physics can be accessed in collision callbacks
        static Villain::SceneNode* WorldNode;
        static BulletEngine* PhysicsWorld;
    private:
        Villain::Camera* camera;
        Villain::DebugRenderer debugRenderer;
        std::unique_ptr<Villain::SkyBox> skybox;

        void createGround();
        void createRigidBody();

        void addPlayer();
        void addRigidBoxes();
        void shootSphere();

        // TEMP: For debugging shadow mapping
        Villain::SpotLight* flashlight;
        Villain::DirectionalLight* directionalLight;
        Villain::PointLight* pointLight1;
        Villain::PointLight* pointLight2;
        Villain::PointLight* pointLight3;
};

#endif // __GAME__
