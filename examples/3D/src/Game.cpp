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
#include "components/ParticlePhysicsComponent.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "physics/BoundingAABB.hpp"
#include "physics/BoundingSphere.hpp"

#include "physics/generators/contact/GroundContacts.hpp"
#include "physics/generators/contact/ParticleCable.hpp"
#include "physics/generators/contact/ParticleRod.hpp"
#include "physics/generators/force/ParticleGravity.hpp"
#include "physics/generators/force/ParticleSpring.hpp"
#include "rendering/DebugRenderer.hpp"
#include <glm/gtx/string_cast.hpp>

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


    // TODO: Move to XML
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
    //addToScene((new SceneNode("Particles"))->addComponent(new ParticleEmitter(100)));

    // V2 - Mass Aggregate particle physics engine tests
    Particle* springA = new Particle();
    Particle* springB = new Particle();
    springA->setPosition({0.0f, 15.0f, 0.0f});
    springB->setPosition({0.0f, 5.0f, 0.0f});

    ParticlePhysicsComponent* particlePhysics = new ParticlePhysicsComponent();
    // ATM component must be added to scene to get pointer to engine so we can setup particles
    addToScene((new SceneNode("Spring test"))->addComponent(particlePhysics));

    // 2 particles connected with spring
    particlePhysics->addParticle(springA);
    particlePhysics->addParticle(springB);
    particlePhysics->addForceGenerator(new ParticleSpring(springB, 1.0f, 2.0f), {0});
    particlePhysics->addForceGenerator(new ParticleSpring(springA, 1.0f, 2.0f), {1});

    Particle* rodA = new Particle();
    Particle* rodB = new Particle();
    rodA->setVelocity({0.0, 0.0, -1.0});
    rodA->setPosition({5.0f, 15.0f, 0.0f});
    rodB->setPosition({5.0f, 5.0f, 0.0f});
    particlePhysics->addParticle(rodA);
    particlePhysics->addParticle(rodB);
    particlePhysics->addContactGenerator(new ParticleRod(9.0f, rodA, rodB));
    //particlePhysics->addContactGenerator(new ParticleCable(8.0f, 1.0f,  rodA, rodB));

    // Generate ground contacts for all particles in component
    particlePhysics->addContactGenerator(new GroundContacts(particlePhysics->getParticles()));

    particlePhysics->addForceGenerator(new ParticleGravity({0.0, -1.0, 0.0}), {3, 2});

    if (getRootNode()->findByID(3)) {
        playerBody = new Particle();
        SceneNode* player = getRootNode()->findByID(3);
        playerBody->setPosition(player->getTransform()->getPos());
        playerBody->setMass(10.0);
        ParticlePhysicsComponent* playerParticleCompo = new ParticlePhysicsComponent(true);
        player->addComponent(playerParticleCompo);
        playerParticleCompo->addParticle(playerBody);
        playerParticleCompo->addContactGenerator(new GroundContacts(playerParticleCompo->getParticles()));
        //playerParticleCompo->addForceGenerator(new ParticleGravity({0.0, -1.0, 0.0}), {0});
    }

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

    EventListener* testListener = new ExampleEventListener();
    getRootNode()->getEngine()->getEventDispatcher()->registerListener(testListener);
}

void Game::handleEvents(float deltaTime) {
    if (Input::Get()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents(dt);
}

void Game::onAppPostUpdate(float dt) {
    //VILLAIN_INFO("Player particle pos {}", glm::to_string(playerBody->getPosition()));
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
}
