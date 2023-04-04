#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "ResourceManager.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"

#include "DebugConsole.hpp"
#include "DebugRenderer.hpp"

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

    // NOTE: P1UV won't cut here anymore if we use lighting system, we need normals as well! :/
    ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "crate");
    std::vector<VertexP1UV> vertices;
    vertices.push_back({glm::vec3(0.0f,  1.0f, -5.0f), glm::vec2(0.5f, 1.0f)});
    vertices.push_back({glm::vec3(1.0f,  0.0f, -5.0f), glm::vec2(1.0f, 0.0f)});
    vertices.push_back({glm::vec3(-1.0f, 0.0f, -5.0f), glm::vec2(0.0f, 0.0f)});
    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Texture*> textures = {ResourceManager::Instance()->getTexture("crate")};
    Mesh<VertexP1UV>* mesh = new Mesh<VertexP1UV>(vertices, indices, textures);
    Material mat("wood", textures, 8);

    planeNode = new SceneNode("Mesh");
    planeNode->addComponent(new MeshRenderer<VertexP1UV>(mesh, mat));

    SceneNode* testHierarchy = new SceneNode("Mesh Child", glm::vec3(0.0f, 3.0f, 0.0f)); // +3 y relative to parent
    testHierarchy->addComponent(new MeshRenderer<VertexP1UV>(mesh, mat));

    SceneNode* testHierarchyChild = new SceneNode("Mesh grandchild", glm::vec3(2.0f, 0.0f, 0.0f)); // +2 x relative to parent
    testHierarchyChild->addComponent(new MeshRenderer<VertexP1UV>(mesh, mat));
    testHierarchy->addChild(testHierarchyChild);

    testHierarchy->getTransform()->setRot(90.f, glm::vec3(0.f, 0.f, 1.f));

    planeNode->addChild(testHierarchy);

    addToScene(planeNode);

    // Add camera
    addToScene((new SceneNode("Main camera"))->addComponent(new CameraComponent(&camera)));

    // Model renderer test
    // 2023-04-04 - Currently ~12FPS with 3 light sources
    SceneNode* modelNode = (new SceneNode("Sponza palace"))->addComponent(new ModelRenderer("assets/models/sponza.obj"));
    modelNode->getTransform()->setScale(0.1f);
    // 2023-04-04 - Currently ~38FPS with 3 light sources
    // Temporary using donut to fix issues
    //SceneNode* modelNode = (new SceneNode("Sponza palace"))->addComponent(new ModelRenderer("assets/models/donut.obj"));
    //modelNode->getTransform()->setScale(4.0f);

    addToScene(modelNode);

    // Light test
    //SceneNode* dirLight = ((new SceneNode("Directional Light 1"))
                //->addComponent(new DirectionalLight(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f),glm::vec3(-0.2f, -1.0f, -0.3f))));
    //addToScene(dirLight);

    glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    SceneNode* pointLight = ((new SceneNode("Point Light 1"))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(1.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight);

    SceneNode* pointLight2 = ((new SceneNode("Point Light 2"))
                ->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.022f, 0.0019f)));
    addToScene(pointLight2);

    glm::vec3 greenLight = glm::vec3(0.0f, 1.0f, 0.f);
    // TODO: somehow will have to make this follow camera, probably needs to be part of same Node
    SceneNode* spotLight = ((new SceneNode("Spot Light"))
                ->addComponent(new SpotLight(greenLight * glm::vec3(0.2f), greenLight, glm::vec3(1.0f), camera.getPosition(), camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)))));
    addToScene(spotLight);


}

Game::~Game() {
    debugRenderer.dispose();
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

    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);

    skybox->render(projection, view);
}
