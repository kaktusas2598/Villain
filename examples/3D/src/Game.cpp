#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/Light.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/ParticleEmitter.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "physics/BoundingAABB.hpp"
#include "physics/BoundingSphere.hpp"

#include "rendering/DebugRenderer.hpp"

using namespace Villain;

// Event Dispatcher Test
#include "events/KeyboardEvent.hpp"
class ExampleEventListener : public EventListener {
    virtual void handleEvent(Event& event) override {
        if (KeyboardEvent* myEvent = dynamic_cast<KeyboardEvent*>(&event)) {
            if (myEvent->isPressed()) {
                printf("KEY Pressed: %d!\n", myEvent->getKey());
            } else {
                printf("KEY Released!\n");
            }
        }
    }
};

void Game::init() {
    loadScene("scene.xml");

    debugRenderer.init();
    GLint result;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &result);
    VILLAIN_INFO("Max vertex shader attrib count is {}", result);

    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &result);
    VILLAIN_INFO("Max uniform location count is {}", result);

    std::vector<std::string> faces{
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };

    skybox = std::make_unique<Villain::SkyBox>(faces, "assets/shaders/cubemap.glsl");

    const unsigned NUM_INSTANCES = 1000;
    std::vector<glm::mat4> instanceTransforms;
    float radius = 5.0f, offset = 0.25f;
    for (unsigned i = 0; i < NUM_INSTANCES; i++) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -100.0f + i * 2.0, 0));
        instanceTransforms.push_back(model);
    }
    Model* wallModel = new Model("assets/models/wall.obj", NUM_INSTANCES, instanceTransforms);
    SceneNode* wall = (new SceneNode("wall", glm::vec3(0.f, 2.f, 0.f)))
        ->addComponent(new ModelRenderer(wallModel));
    wall->getTransform()->setEulerRot(0.0f, 0.f, 90.f);
    addToScene(wall);

    // TODO: Lights TEMP disabled
    // Light test - Cause of the biggest FPS drop in the Engine! (Especially when using more than 1 light source)
    SceneNode* directionalLight = ((new SceneNode("Directional Light 1", glm::vec3(10, 10, 10)))
                ->addComponent(new DirectionalLight(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f),glm::vec3(-0.2f, -0.8f, -0.5f))));
    addToScene(directionalLight);

    //glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    //SceneNode* pointLight = ((new SceneNode("Point Light 1", glm::vec3(4.f, 2.f, 3.f)))
                //->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(100.0f, 2.0f, -10.0f), glm::vec3(1.0f, 0.022f, 0.0019f))));
    //pointLight->addChild(planeNode);
    //addToScene(pointLight);

    //PointLight* pointLight2 = new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(1.0f, 0.022f, 0.0019f));
    //SceneNode* point2 = ((new SceneNode("Point Light 2"))->addComponent(pointLight2));
    //addToScene(point2);

    //glm::vec3 lightColor = glm::vec3(0.5f, 0.7f, 0.4f);
    //SceneNode* spotLight = ((new SceneNode("Spot Light"))
                //->addComponent(new SpotLight(lightColor * glm::vec3(0.2f), lightColor, glm::vec3(1.0f), glm::vec3(20.f, 20.f, 10.f), glm::vec3(0.0f, -5.f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), glm::vec3(1.0f, 0.022f, 0.0019f), &camera)));
    //addToScene(spotLight);


    // Physics demo
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingSphere(glm::vec3(-50.0f, 4.5f, 0.f), 1.0f), 1.0f, glm::vec3(6.0f, 0.f, 0.f)));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingSphere(glm::vec3(50.0f, 5.0f, 0.f), 1.0f), 1.0f, glm::vec3(-6.0f, 0.f, 0.f)));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(120.0, -20.0, -50.0), glm::vec3(122.0, 50.0, 50.0)), 0.0f));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(-142.0, -20.0, -50.0), glm::vec3(-140.0, 50.0, 50.0)), 0.0f));
    // FIXME: adding floor causes spheres to dissapear?
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(-240.0, 0.0, -50.0), glm::vec3(240.0, -1.0, 50.0)), 0.0f));

    // New Particle Engine tests
    addToScene((new SceneNode("Particles"))->addComponent(new ParticleEmitter(100)));

    // TODO: need to make it easier to add physics object to physics engine and then to scene graph, easier way to find a particular object
    Model* sphereModel = new Model("assets/models/sphere.obj");
    addToScene((new SceneNode("physics object 0"))
        ->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        ->addComponent(new ModelRenderer(sphereModel)));
    addToScene((new SceneNode("physics object 1"))
        ->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        ->addComponent(new ModelRenderer(sphereModel)));
    addToScene((new SceneNode("AABB"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(2))));
    addToScene((new SceneNode("AABB2"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(3))));
    addToScene((new SceneNode("Floor"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(4))));

    printf("Camera ID: %i\n", GetId<Camera>());
    printf("ModelRenderer ID: %i\n", GetId<ModelRenderer>());
    printf("PhysicsObjectComponent ID: %i\n", GetId<PhysicsObjectComponent>());

    /*
    // Skeletal Animation demo
    Model* animatedModel = new Model("assets/models/mudeater.dae");
    SceneNode* animatedNode = (new SceneNode("Animated Model", glm::vec3(12, 0, 0)))->addComponent(new ModelRenderer(animatedModel));
    animatedNode->getTransform()->setEulerRot(-90.0f, -90.0f, 0.0f);
    addToScene(animatedNode);

    //Model* catModel = new Model("assets/models/AnimalPackVol2Quaternius/FBX/Cat.fbx");
    //SceneNode* catNode = (new SceneNode("Cat", {0, 0, 6}))->addComponent(new ModelRenderer(catModel));
    //catNode->getTransform()->setScale(0.02);
    //catNode->getTransform()->setEulerRot(0.0f, -180.0f, 0.0f);
    //addToScene(catNode);

    std::string eaglePath = "assets/models/AnimalPackVol2Quaternius/Eagle.fbx";
    Model* eagleModel = new Model(eaglePath.c_str());
    SceneNode* eagleNode = (new SceneNode("Eagle", {12, 20, 0}))->addComponent(new ModelRenderer(eagleModel));
    eagleNode->getTransform()->setScale(0.02);
    eagleNode->getTransform()->setEulerRot(0.0f, 90.0f, 0.0f);
    addToScene(eagleNode);

    //std::string vampirePath = "assets/models/Rumba Dancing.fbx";
    std::string vampirePath = "assets/models/ThrillerPart1-Vampire.dae";
    Model* vampireModel = new Model(vampirePath.c_str());
    SceneNode* vampireNode = (new SceneNode("Dancing Vampire", glm::vec3(12, 0, 6)))->addComponent(new ModelRenderer(vampireModel, 420));
    vampireNode->getTransform()->setScale(0.05);
    vampireNode->getTransform()->setEulerRot(0.0f, -90.0f, 0.0f);
    addToScene(vampireNode);

    // This is part 4, also works, but fbx binary file has wrong texture paths
    //std::string thrillerPath = "assets/models/Thriller.fbx";
    //std::string thrillerPath = "assets/models/Angry.fbx";
    std::string thrillerPath = "assets/models/ThrillerPart1-ZombieGirl.dae";
    Model* thrillerModel = new Model(thrillerPath.c_str());
    SceneNode* thrillerNode = (new SceneNode("thriller", {12, 0, -6}))->addComponent(new ModelRenderer(thrillerModel, 420));
    thrillerNode->getTransform()->setScale(0.05);
    thrillerNode->getTransform()->setEulerRot(0.0f, -90.0f, 0.0f);
    addToScene(thrillerNode);
    */

    EventListener* testListener = new ExampleEventListener();
    getRootNode()->getEngine()->getEventDispatcher()->registerListener(testListener);
}

void Game::handleEvents(float deltaTime) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents(dt);
}

void Game::onAppPostUpdate(float dt) {
}

void Game::onAppRender(float dt) {
    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    debugRenderer.end();
    Camera* mainCamera = getRootNode()->getEngine()->getRenderingEngine()->getMainCamera();
    glm::mat4 view = mainCamera->getViewMatrix();
    glm::mat4 projection = mainCamera->getProjMatrix();

    debugRenderer.render(projection * view, 1.0f);

    skybox->render(projection, view);
}
