#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "Camera3D.hpp"
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
        Villain::Camera3D camera;
        Villain::DebugRenderer debugRenderer;
        std::unique_ptr<Villain::SkyBox> skybox;

        void createGround();
        void createRigidBody();

        void addPlayer();
        void addRigidBoxes();
        void shootSphere();

        glm::vec3 mouseRayToWorld(const glm::vec2 & pos);

        // TEMP: For debugging shadow mapping
        Villain::SpotLight* flashlight;
        Villain::DirectionalLight* directionalLight;
};

#endif // __GAME__
