#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/KinematicController.hpp"
#include "components/Light.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/ParticleEmitter.hpp"
#include "components/ParticlePhysicsComponent.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "components/RigidBodyComponent.hpp"
#include "physics/BoundingVolume.hpp"

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

    // Instancing test
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


    // Physics demo
    //getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingSphere(glm::vec3(-50.0f, 4.5f, 0.f), 1.0f), 1.0f, glm::vec3(6.0f, 0.f, 0.f)));
    //getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingSphere(glm::vec3(50.0f, 5.0f, 0.f), 1.0f), 1.0f, glm::vec3(-6.0f, 0.f, 0.f)));
    //getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(120.0, -20.0, -50.0), glm::vec3(122.0, 50.0, 50.0)), 0.0f));
    //getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(-142.0, -20.0, -50.0), glm::vec3(-140.0, 50.0, 50.0)), 0.0f));
    //// FIXME: adding floor causes spheres to dissapear?
    //getRootNode()->getEngine()->getPhysicsEngine()->addObject(new PhysicsObject(new BoundingAABB(glm::vec3(-240.0, 0.0, -50.0), glm::vec3(240.0, -1.0, 50.0)), 0.0f));

    // New Particle Engine tests
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

    // Rigid Body test
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


    // TODO: need to make it easier to add physics object to physics engine and then to scene graph, easier way to find a particular object
    //Model* sphereModel = new Model("assets/models/sphere.obj");
    //addToScene((new SceneNode("physics object 0"))
        //->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        //->addComponent(new ModelRenderer(sphereModel)));
    //addToScene((new SceneNode("physics object 1"))
        //->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        //->addComponent(new ModelRenderer(sphereModel)));
    //addToScene((new SceneNode("AABB"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(2))));
    //addToScene((new SceneNode("AABB2"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(3))));
    //addToScene((new SceneNode("Floor"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(4))));

    //printf("Camera ID: %i\n", GetId<Camera>());
    //printf("ModelRenderer ID: %i\n", GetId<ModelRenderer>());
    //printf("PhysicsObjectComponent ID: %i\n", GetId<PhysicsObjectComponent>());

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
    //VILLAIN_INFO("Player particle pos {}", glm::to_string(playerBody->getPosition()));


    // NOTE: Running simulation for specified number of frames for debugging reasons
    ////////////////////////////
    //const unsigned maxFrames = 680;
    //static unsigned counter = 0;
    //if (counter > maxFrames) {
        //Engine::setRunning(false);
    //}
    //counter++;
    ////////////////////////////
}

void Game::onAppRender(float dt) {
    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Mock up grid, just a demonstration, we need
    debugRenderer.drawQuad(glm::vec3(-3.f, 0.f, -3.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(-3.f, 0.f, 3.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(-3.f, 0.f, 0.f), glm::vec2(3.0, 3.0), true, false, true);

    debugRenderer.drawQuad(glm::vec3(0.f, 0.f, -3.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(0.f, 0.f, 0.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(0.f, 0.f, 3.f), glm::vec2(3.0, 3.0), true, false, true);

    debugRenderer.drawQuad(glm::vec3(3.f, 0.f, -3.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(3.f, 0.f, 0.f), glm::vec2(3.0, 3.0), true, false, true);
    debugRenderer.drawQuad(glm::vec3(3.f, 0.f, 3.f), glm::vec2(3.0, 3.0), true, false, true);

    ////////////////////////////////////////
    /// Collision narrow phase tests
    //CollisionBox box({10.0, 2.0, 8.0}, playerBody);

    // Define the CollisionPlane for the ground (Y level = 0)
    //CollisionPlane groundPlane({0.0, 1.0, 0.0}, 0.0);

    // Perform collision detection between the camera's bounding box and the ground plane
    //Contact* contactArray = new Contact[24];
    //CollisionData collisionData(contactArray, 24, 0, 0.5f, 0.0f);

    // Box-plane test
    // Check if any contacts were found
    //if (CollisionDetector::boxAndHalfSpace(*box, groundPlane, &collisionData)) {
    //if (CollisionDetector::sphereAndHalfSpace(sphere, groundPlane, &collisionData)) {
        // Collision occurred, change the color of the box to indicate the collision
        // (You may need to implement this logic in your rendering system)
        //VILLAIN_DEBUG("Collision Detected!!");
        //debugRenderer.drawBox3D(playerBody->getPosition(), {1.0, 0.2, 0.2, 1.0}, box->halfSize * 2.0f);
        //debugRenderer.drawSphere(playerBody->getPosition(), sphere.radius, {1.0, 0.2, 0.2, 1.0});
    //} else {
        // No collision, render the box with the default color
        //debugRenderer.drawBox3D(playerBody->getPosition(), {0.4, 0.7, 0.2, 1.0}, box->halfSize * 2.0f);
        //debugRenderer.drawSphere(playerBody->getPosition(), sphere.radius, {0.4, 0.7, 0.2, 1.0});
    //}

    //CollisionSphere one{2.0f, sphere1};
    //one.body->calculateDerivedData();
    //CollisionSphere two{1.0f, sphere2};
    //two.body->calculateDerivedData();

    // Sphere-sphere and box-sphere tests
    //if (CollisionDetector::boxAndSphere(*box, *one, &collisionData)) {
        //debugRenderer.drawSphere(one->body->getPosition(), one->radius, {1.0, 0.0, 0.0, 1.0});
    //} else {
        //if (CollisionDetector::sphereAndSphere(*one, *two, &collisionData)) {
            //debugRenderer.drawSphere(one->body->getPosition(), one->radius, {0.4, 0.7, 0.2, 1.0});
            //debugRenderer.drawSphere(two->body->getPosition(), two->radius, {1.0, 0.1, 0.6, 1.0});
        //} else {
            //debugRenderer.drawSphere(one->body->getPosition(), one->radius);
            //debugRenderer.drawSphere(two->body->getPosition(), two->radius);
        //}
    //}


    //// Box-box SAT tests
    //CollisionBox staticBox({3.0, 2.0, 5.0}, boxBody);
    //boxBody->calculateDerivedData();
    //if (CollisionDetector::boxAndBox(*box, staticBox, &collisionData)) {
        ////debugRenderer.drawBox3D(boxBody->getPosition(), {1.0, 0.0, 0.0, 1.0}, staticBox.halfSize * 2.0f);
        //debugRenderer.drawBox3DRotated(boxBody->getPosition(), staticBox.halfSize * 2.0f, glm::mat4(boxBody->getOrientation()), {1.0, 0.0, 0.0, 1.0});
    //} else {
        ////debugRenderer.drawBox3D(boxBody->getPosition(), {0.4, 0.7, 0.2, 1.0}, staticBox.halfSize * 2.0f);
        //debugRenderer.drawBox3DRotated(boxBody->getPosition(), staticBox.halfSize * 2.0f, glm::mat4(boxBody->getOrientation()), {0.4, 0.7, 0.2, 1.0});
    //}

    // Don't forget to clean up the allocated memory for contacts
    //delete[] collisionData.contacts;
    ////////////////////////////////////////

    debugRenderer.end();
    Camera* mainCamera = getRootNode()->getEngine()->getRenderingEngine()->getMainCamera();

    glm::mat4 view = mainCamera->getViewMatrix();
    glm::mat4 projection = mainCamera->getProjMatrix();

    debugRenderer.render(projection * view, 1.0f);
}
