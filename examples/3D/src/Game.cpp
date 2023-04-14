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
#include "physics/BoundingSphere.hpp"

#include "DebugConsole.hpp"
#include "rendering/DebugRenderer.hpp"

using namespace Villain;

void Game::init() {
    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

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

    ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate");
    std::vector<VertexP1N1UV> vertices;
    // nomals here are wrong cause im lazy, just wanted to get uv working cause it's 2nd vertex attrib, so need normals
    vertices.push_back({glm::vec3(0.0f,  1.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 1.0f)});
    vertices.push_back({glm::vec3(1.0f,  0.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)});
    vertices.push_back({glm::vec3(-1.0f, 0.0f, -5.0f),glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});
    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Texture*> textures = {ResourceManager::Instance()->getTexture("crate")};
    Material mat("wood", textures, 8);
    Mesh<VertexP1N1UV>* mesh = new Mesh<VertexP1N1UV>(vertices, indices);

    SceneNode* planeNode = (new SceneNode("Mesh"))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchy = (new SceneNode("Mesh Child", glm::vec3(0.0f, 3.0f, 0.0f)))->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));

    SceneNode* testHierarchyChild = new SceneNode("Mesh grandchild", glm::vec3(2.0f, 0.0f, 0.0f));
    testHierarchyChild->addComponent(new MeshRenderer<VertexP1N1UV>(mesh, mat));
    //testHierarchy->addChild(testHierarchyChild);

    testHierarchy->getTransform()->setEulerRot(45.0f, 0.f, 0.f);

    //planeNode->addChild(testHierarchy);

    //addToScene(planeNode);

    // Add camera
    SceneNode* cam = (new SceneNode("Free look camera"))
            ->addComponent(new CameraComponent(&camera))
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

    // Light test
    //SceneNode* dirLight = ((new SceneNode("Directional Light 1"))
                //->addComponent(new DirectionalLight(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f),glm::vec3(-0.2f, -1.0f, -0.3f))));
    //addToScene(dirLight);

    glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    SceneNode* pointLight = ((new SceneNode("Point Light 1", glm::vec3(4.f, 2.f, 3.f)))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(100.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f)));
    pointLight->addChild(planeNode);
    addToScene(pointLight);

    SceneNode* pointLight2 = ((new SceneNode("Point Light 2"))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight2);

    //SceneNode* pointLights;
    //for (int i = 0; i < 5; i++) {
        //pointLights = ((new SceneNode(std::to_string(i)))
                //->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f * i, 5.0f, 5.0f), 1.0f, 0.022f, 0.0019f)));
        //addToScene(pointLights);

    //}

    glm::vec3 lightColor = glm::vec3(0.5f, 0.7f, 0.4f);
    SceneNode* spotLight = ((new SceneNode("Spot Light"))
                ->addComponent(new SpotLight(lightColor * glm::vec3(0.2f), lightColor, glm::vec3(1.0f), glm::vec3(20.f, 20.f, 10.f), glm::vec3(0.0f, -5.f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))/*, &camera*/)));
    addToScene(spotLight);


    // Physics demo
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(-50.0f, 4.5f, 0.f), 1.0f), glm::vec3(6.0f, 0.f, 0.f)));
    getRootNode()->getEngine()->getPhysicsEngine()->addObject(PhysicsObject(new BoundingSphere(glm::vec3(50.0f, 5.0f, 0.f), 1.0f), glm::vec3(-6.0f, 0.f, 0.f)));

    // TODO: need to make it easier to add physics object to physics engine and then to scene graph, easier way to find a particular object
    addToScene((new SceneNode("physics object 0"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(0)))
        ->addComponent(new ModelRenderer("assets/models/sphere.obj")));
    addToScene((new SceneNode("physics object 1"))
        ->addComponent(new PhysicsObjectComponent(&getRootNode()->getEngine()->getPhysicsEngine()->getObject(1)))
        ->addComponent(new ModelRenderer("assets/models/sphere.obj")));
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
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjMatrix();

    debugRenderer.drawBox3D(glm::vec3(0.0f, 2.5f, -12.0f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec3(10.0f, 5.0f, 0.1f));
    debugRenderer.drawBox3D(glm::vec3(5.0f, 2.5f, -5.0f), glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), glm::vec3(0.1f, 5.0f, 10.0f));
    debugRenderer.drawBox(glm::vec4(0.0f, 0.0f, 2.0f, 2.0f), -5.0f, glm::vec4(1.0f), 0.0f);

    debugRenderer.drawSphere(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec4(0.8f, 0.0f, 2.0f, 2.0f), 5.0f);

    debugRenderer.drawLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 10.f, 0.f));

    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);

    skybox->render(projection, view);
}
