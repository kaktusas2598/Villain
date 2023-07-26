#include "Game.hpp"

#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/KinematicController.hpp"
#include "components/Light.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/ParticleEmitter.hpp"
#include "components/ParticlePhysicsComponent.hpp"
#include "components/RigidBodyComponent.hpp"

#include "physics/NarrowPhase.hpp"
#include "physics/generators/contact/GroundContacts.hpp"
#include "physics/generators/contact/ParticleCable.hpp"
#include "physics/generators/contact/ParticleRod.hpp"
#include "physics/generators/force/ParticleGravity.hpp"
#include "physics/generators/force/ParticleSpring.hpp"
#include "physics/generators/force/Gravity.hpp"
#include "physics/generators/force/Spring.hpp"
#include "rendering/DebugRenderer.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Villain;

// Event Dispatcher Test
#include "events/KeyboardEvent.hpp"
class ExampleEventListener : public EventListener {
    virtual void handleEvent(Event& event) override {
        if (KeyboardEvent* myEvent = dynamic_cast<KeyboardEvent*>(&event)) {
            if (myEvent->isPressed()) {
                //printf("KEY Pressed: %c!\n", (char)myEvent->getKey());
            } else {
                //printf("KEY Released!\n");
            }
        }
    }
};

void Game::init() {
    loadScene("scene.xml");
    debugRenderer.init();

    ///////////////////////////////////////////////////////
    // Instancing test
    ///////////////////////////////////////////////////////
    //const unsigned NUM_INSTANCES = 1000;
    //std::vector<glm::mat4> instanceTransforms;
    //float radius = 5.0f, offset = 0.25f;
    //for (unsigned i = 0; i < NUM_INSTANCES; i++) {
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -100.0f + i * 2.0, 0));
        //instanceTransforms.push_back(model);
    //}
    //Model* wallModel = new Model("assets/models/wall.obj", NUM_INSTANCES, instanceTransforms);
    //SceneNode* wall = (new SceneNode("wall", glm::vec3(0.f, 2.f, 0.f)))
        //->addComponent(new ModelRenderer(wallModel));
    //wall->getTransform()->setEulerRot(0.0f, 0.f, 90.f);
    //addToScene(wall);


    ///////////////////////////////////////////////////////
    // New Particle Engine tests
    ///////////////////////////////////////////////////////
    //addToScene((new SceneNode("Particles"))->addComponent(new ParticleEmitter(100)));

    // V2 - Mass Aggregate particle physics engine tests
    //Particle* springA = new Particle();
    //Particle* springB = new Particle();
    //springA->setPosition({0.0f, 15.0f, 0.0f});
    //springB->setPosition({0.0f, 5.0f, 0.0f});

    //ParticlePhysicsComponent* particlePhysics = new ParticlePhysicsComponent();
    //// ATM component must be added to scene to get pointer to engine so we can setup particles
    //addToScene((new SceneNode("Spring test"))->addComponent(particlePhysics));

    //// 2 particles connected with spring
    //particlePhysics->addParticle(springA);
    //particlePhysics->addParticle(springB);
    //particlePhysics->addForceGenerator(new ParticleSpring(springB, 1.0f, 2.0f), {0});
    //particlePhysics->addForceGenerator(new ParticleSpring(springA, 1.0f, 2.0f), {1});

    //Particle* rodA = new Particle();
    //Particle* rodB = new Particle();
    //rodA->setVelocity({0.0, 0.0, -1.0});
    //rodA->setPosition({5.0f, 15.0f, 0.0f});
    //rodB->setPosition({5.0f, 5.0f, 0.0f});
    //particlePhysics->addParticle(rodA);
    //particlePhysics->addParticle(rodB);
    //particlePhysics->addContactGenerator(new ParticleRod(9.0f, rodA, rodB));
    ////particlePhysics->addContactGenerator(new ParticleCable(8.0f, 1.0f,  rodA, rodB));

    //// Generate ground contacts for all particles in component
    //particlePhysics->addContactGenerator(new GroundContacts(particlePhysics->getParticles()));

    //particlePhysics->addForceGenerator(new ParticleGravity({0.0, -1.0, 0.0}), {3, 2});

    // //////////////////////////////
    //glm::vec3* windSpeed = new glm::vec3(0, 0, 0);
    if (getRootNode()->findByID(1)) {
        //playerBody = new Particle();
        SceneNode* player = getRootNode()->findByID(1);
        //playerBody->setPosition(player->getTransform()->getPos());
        //playerBody->setMass(10.0);
        //ParticlePhysicsComponent* playerParticleCompo = new ParticlePhysicsComponent(true);
        //player->addComponent(playerParticleCompo);
        //playerParticleCompo->addParticle(playerBody);
        //playerParticleCompo->addContactGenerator(new GroundContacts(playerParticleCompo->getParticles()));

        //aircraft = new FlightController(new RigidBody(), windSpeed);
        //player->addComponent(aircraft);

        playerBody = new RigidBody();
        playerBody->setPosition({-20, 10, 0});
        CollisionBox* box = new CollisionBox({10.0, 2.0, 8.0}, playerBody);
        KinematicController* playerControl = new KinematicController(playerBody, box);
        player->addComponent(playerControl);
        //player->addComponent(new ModelRenderer(new Model("assets/models/toyplane.obj")));
    }

    // Setting up meshes for rigid bodies test
    std::vector<VertexP1N1T1B1UV> vertices;
    std::vector<unsigned int> indices;
    MeshUtils<VertexP1N1T1B1UV>::addAABB(&vertices, &indices); // 1x1x1 cube centered on 0,0,0
    Mesh<VertexP1N1T1B1UV>* cubeMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);
    vertices.clear(); indices.clear();
    MeshUtils<VertexP1N1T1B1UV>::addSphere(&vertices, &indices, 1.0f); // Sphere with radius of 1
    Mesh<VertexP1N1T1B1UV>* sphereMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);

    ///////////////////////////////////////////////////////
    // Rigid Body test
    ///////////////////////////////////////////////////////
    RigidBody* rigidBody1 = new RigidBody();
    RigidBody* rigidBody2 = new RigidBody();
    rigidBody1->setPosition({2.0, 5.0, 2.0});
    rigidBody2->setPosition({2.0, 0.0, 2.0});

    CollisionSphere* sphere = new CollisionSphere(1.0f, rigidBody1);
    box = new CollisionBox({0.5, 0.5, 0.5}, rigidBody2);

    RigidBodyComponent* rigidBodyCompo1 = new RigidBodyComponent(rigidBody1/*, sphere*/);
    RigidBodyComponent* rigidBodyCompo2 = new RigidBodyComponent(rigidBody2/*, box*/);
    addToScene((new SceneNode("Rigid Body 1"))->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(sphereMesh, Material()))->addComponent(rigidBodyCompo1));
    addToScene((new SceneNode("Rigid Body 2"))->addComponent(new MeshRenderer<VertexP1N1T1B1UV>(cubeMesh, Material()))->addComponent(rigidBodyCompo2));

    // Attach 2 bodies with a spring
    rigidBodyCompo1->addForceGenerator(new Spring({0, 0, 0}, rigidBody2, {0.2, 0, 0}, 1.0f, 2.0f));
    rigidBodyCompo2->addForceGenerator(new Spring({0.2, 0, 0}, rigidBody1, {0, 0, 0}, 1.0f, 2.0f));

    //printf("Camera ID: %i\n", GetId<Camera>());
    //printf("ModelRenderer ID: %i\n", GetId<ModelRenderer>());

    EventListener* testListener = new ExampleEventListener();
    getRootNode()->getEngine()->getEventDispatcher()->registerListener(testListener);

    sphere1 = new RigidBody();
    sphere2 = new RigidBody();
    boxBody = new RigidBody();

    one = new CollisionSphere({2.0f, sphere1});
    two = new CollisionSphere({1.0f, sphere2});

    sphere1->setPosition({10.0f, 5.0f, -20.0f});
    sphere1->setLinearVelocity({0.0, 0.0, 2.0});
    sphere2->setPosition({10.0f, 5.0f, 20.0f});
    sphere2->setLinearVelocity({0.0, 0.0, -2.0});
    boxBody->setOrientation(glm::quat(glm::vec3(0.0, 45.0, 0.0)));

    RigidBodyComponent* rigidBodyCompo3 = new RigidBodyComponent(sphere1, one);
    RigidBodyComponent* rigidBodyCompo4 = new RigidBodyComponent(sphere2, two);
    addToScene((new SceneNode("Rigid Body 3"))->addComponent(rigidBodyCompo3));
    addToScene((new SceneNode("Rigid Body 4"))->addComponent(rigidBodyCompo4));

    // Define Ground
    CollisionPlane* groundPlane = new CollisionPlane({0.0, 1.0, 0.0}, 0.0);
    getRootNode()->getEngine()->getRigidBodyWorld()->getColliders().push_back(groundPlane);

    // Add gravity for all bodies
    //Gravity* gravity = new Gravity({0.0, -1.0, 0.0});
    //for (auto& body: getRootNode()->getEngine()->getRigidBodyWorld()->getBodies()) {
        //getRootNode()->getEngine()->getRigidBodyWorld()->getForceRegistry().add(body, gravity);
    //}

    getRootNode()->getEngine()->getRigidBodyWorld()->setDebugDraw(true);

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
}

void Game::onAppRender(float dt) {
    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Render ground plane
    debugRenderer.drawGrid(glm::vec3(0.f, 1.f, 0.f), 0.0f, 100, 100, 1.0f, glm::vec4(1.0, 0.0, 0.0, 1.0));

    ///////////////////////////////////////////////////////
    // Bezier curve demo
    ///////////////////////////////////////////////////////
    glm::vec3 origin(0.0f, 8.0f, 0.0f);
    glm::vec3 control(2.0f, 6.0f, 0.0f);
    glm::vec3 end(14.0f, 2.0f, 0.0f);
    int numSegments = 50; // You can adjust this value to control the smoothness of the curve
    debugRenderer.drawQuadraticBezierCurve(origin, control, end, numSegments, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    debugRenderer.end();
    Camera* mainCamera = getRootNode()->getEngine()->getRenderingEngine()->getMainCamera();

    glm::mat4 view = mainCamera->getViewMatrix();
    glm::mat4 projection = mainCamera->getProjMatrix();

    debugRenderer.render(projection * view, 1.0f);
}
