#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/Light.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "physics/BoundingAABB.hpp"
#include "physics/BoundingSphere.hpp"

#include "DebugConsole.hpp"
#include "rendering/DebugRenderer.hpp"

using namespace Villain;

void Game::init() {
    camera = new Camera();
    camera->setZPlanes(0.1f, 1000.f); // for bigger render range
    camera->rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    debugRenderer.init();

    std::vector<std::string> faces{
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };

    skybox = std::make_unique<Villain::SkyBox>(faces, "assets/shaders/cubemap.glsl");

    //ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate");
    std::vector<VertexP1N1UV> vertices;
    // nomals here are wrong cause im lazy, just wanted to get uv working cause it's 2nd vertex attrib, so need normals
    vertices.push_back({glm::vec3(0.0f,  1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 1.0f)});
    vertices.push_back({glm::vec3(1.0f,  0.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
    vertices.push_back({glm::vec3(-1.0f, 0.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
    std::vector<unsigned int> indices = {0, 1, 2};
    //std::vector<Texture*> textures = {ResourceManager::Instance()->getTexture("crate")};
    Material mat("wood", ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate"), 8);
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    SceneNode* planeNode = (new SceneNode("Mesh"))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchy = (new SceneNode("Mesh Child", glm::vec3(0.0f, 3.0f, 0.0f)))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchyChild = new SceneNode("Mesh grandchild", glm::vec3(2.0f, 0.0f, 0.0f));
    testHierarchyChild->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    //testHierarchy->addChild(testHierarchyChild);

    testHierarchy->getTransform()->setEulerRot(45.0f, 0.f, 0.f);

    //planeNode->addChild(testHierarchy);

    //addToScene(planeNode);

    //SceneNode* cam = (new SceneNode("Free look camera"))->addComponentNew<CameraComponent>(camera);
    // Add camera
    SceneNode* cam = (new SceneNode("Free look camera"))
            ->addComponent(new CameraComponent(camera))
            ->addComponent(new MoveController())
            ->addComponent(new LookController());
    addToScene(cam);

    // Model renderer test
    // 2023-04-04 - Currently ~12FPS with 3 light sources
    // WIth frustum culling seeing increases from ~3 to ~10FPS, still not enough
    SceneNode* modelNode = (new SceneNode("Sponza palace"))->addComponent(new ModelRenderer("assets/models/sponza.obj"));
    modelNode->getTransform()->setScale(0.1f);
    // 2023-04-04 - Currently ~38FPS with 3 light sources
    // Temporary using donut to fix issues
    //modelNode->getTransform()->setScale(4.0f);
    addToScene(modelNode);

    SceneNode* rockNode = (new SceneNode("Rock", glm::vec3(2.f, 1.f, 0.f)))->addComponent(new ModelRenderer("assets/models/rock.obj"));
    rockNode->getTransform()->setScale(0.01f);
    addToScene(rockNode);

    SceneNode* wall = (new SceneNode("wall", glm::vec3(4.f, 1.f, 0.f)))->addComponent(new ModelRenderer("assets/models/wall.obj"));
    wall->getTransform()->setEulerRot(0.0f, 0.f, 90.f);
    addToScene(wall);

    // Light test - Cause of the biggest FPS drop in the Engine! (Especially when using more than 1 light source)
    directionalLight = ((new SceneNode("Directional Light 1", glm::vec3(10, 10, 10)))
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

    // TODO: need to make it easier to add physics object to physics engine and then to scene graph, easier way to find a particular object
    addToScene((new SceneNode("physics object 0"))
        ->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        ->addComponent(new ModelRenderer("assets/models/sphere.obj")));
    addToScene((new SceneNode("physics object 1"))
        ->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        ->addComponent(new ModelRenderer("assets/models/sphere.obj")));
    addToScene((new SceneNode("AABB"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(2))));
    addToScene((new SceneNode("AABB2"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(3))));
    addToScene((new SceneNode("Floor"))->addComponent(new PhysicsObjectComponent(getRootNode()->getEngine()->getPhysicsEngine()->getObject(4))));

    printf("CameraComponent ID: %i\n", GetId<CameraComponent>());
    printf("ModelRenderer ID: %i\n", GetId<ModelRenderer>());
    printf("PhysicsObjectComponent ID: %i\n", GetId<PhysicsObjectComponent>());
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
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjMatrix();

    // Draw coordinate gizmo
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(5.f, 0.f, 0.f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Draw example shapes test
    debugRenderer.drawBox3D(glm::vec3(5.0f, 2.5f, -5.0f), glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), glm::vec3(0.1f, 5.0f, 10.0f));
    debugRenderer.drawBox(glm::vec4(0.0f, 0.0f, 2.0f, 2.0f), -5.0f, glm::vec4(1.0f), 0.0f);
    debugRenderer.drawSphere(glm::vec3(2.0f, 6.0f, -10.0f), 5.0f, glm::vec4(0.8f, 0.0f, 1.0f, 1.0f));

    glm::vec3 position = glm::vec3(2.0);
    glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = transformY * transformX * transformZ;
    debugRenderer.drawBox3D(position, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
    debugRenderer.drawBox3DRotated(position, glm::vec3(1.0f), rotation, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));


    // Directional light has no position, but we need to set some sort of position for shadow map! So this
    // is just a representation os such position
    //debugRenderer.drawSphere(directionalLight->getTransform()->getPos(), 1.0f, glm::vec4(0.8f, 1.0f, 0.0f, 1.0f));

    debugRenderer.end();
    debugRenderer.render(projection * view, 1.0f);

    skybox->render(projection, view);
}
