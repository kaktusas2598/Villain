#include "Game.hpp"

#include "Light.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "physics/BoundingSphere.hpp"

#include "Door.hpp"
#include "Level.hpp"
#include "Player.hpp"

using namespace Villain;


void Game::init() {
    SceneNode* wall = (new SceneNode("wall", glm::vec3(4.f, 1.f, 0.f)))->addComponent(new ModelRenderer("assets/models/wall.obj"));
    addToScene(wall);

    // Light test
    glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    SceneNode* pointLight = ((new SceneNode("Point Light 1", glm::vec3(4.f, 2.f, 3.f)))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(100.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight);

    SceneNode* pointLight2 = ((new SceneNode("Point Light 2"))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight2);

    glm::vec3 lightColor = glm::vec3(0.5f, 0.7f, 0.4f);
    SceneNode* spotLight = ((new SceneNode("Spot Light"))
                ->addComponent(new SpotLight(lightColor * glm::vec3(0.2f), lightColor, glm::vec3(1.0f), glm::vec3(20.f, 20.f, 10.f), glm::vec3(0.0f, -5.f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))/*, &camera*/)));
    addToScene(spotLight);


    // Physics demo
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(-50.0f, 4.5f, 0.f), 1.0f), glm::vec3(3.0f, 0.f, 0.f)));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(50.0f, 5.0f, 0.f), 1.0f), glm::vec3(-3.0f, 0.f, 0.f)));

    addToScene((new SceneNode("physics object 0"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        ->addComponent(new ModelRenderer("assets/models/wall.obj")));
    addToScene((new SceneNode("physics object 1"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        ->addComponent(new ModelRenderer("assets/models/wall.obj")));

    // Add level which will also generate and add to scene all special objects
    currentLevel = new Level("assets/textures/level1.png", "assets/textures/WolfCollection.png", this);

    // Add camera and player
    camera = new Camera3D();
    camera->setZPlanes(0.1f, 1000.f); // for bigger render range

    SceneNode* player = (new SceneNode("Player", glm::vec3(3.f, 1.f, 17.5f)))->addComponent(new CameraComponent(camera));
    player->addComponent(new Player(currentLevel));
    player->addComponent(new LookController());
    //player->addComponent(new MoveController());
    addToScene(player);
}

Game::~Game() {
}

void Game::onAppPreUpdate(float dt) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}
