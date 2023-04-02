#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"
#include "components/CameraComponent.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "DebugConsole.hpp"
#include "DebugRenderer.hpp"

using namespace Villain;

void Game::init() {
    //model3D = new Model("assets/models/donut.obj");
    model3D = new Model("assets/models/sponza.obj");

    camera.setZPlanes(0.1f, 1000.f); // for bigger render range
    camera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());

    ResourceManager::Instance()->loadShader("assets/shaders/models.glsl", "model");

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
    SceneNode* modelNode = (new SceneNode("Sponza palace"))->addComponent(new ModelRenderer("assets/models/sponza.obj"));
    modelNode->getTransform()->setScale(0.1f);
    addToScene(modelNode);
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

    //glm::vec3 newPos = planeNode->getTransform()->getPos();
    //newPos.x += 0.0001f;
    //planeNode->getTransform()->setPos(newPos);
}

void Game::onAppPostUpdate(float dt) {
}

void Game::onAppRender(float dt) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -5.0f));
    //model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.1f)); // for sponza
    //model = glm::scale(model, glm::vec3(4.0f)); // for donut
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjMatrix();

    /*BaseLight dirColor(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f));
    DirectionalLight dirLight(dirColor, glm::vec3(-0.2f, -1.0f, -0.3f));

    glm::vec3 spotLightColor = glm::vec3(1.0f, 1.0f, 0.0f);
    BaseLight spotColor(spotLightColor * glm::vec3(0.2f), spotLightColor * glm::vec3(0.9f), glm::vec3(1.0f));
    // Make spot light position same as camera thus simulating flashlight!
    SpotLight spotLight(spotColor, camera.getPosition(), camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

    glm::vec3 pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
    BaseLight pointColor(pointLightColor * glm::vec3(0.5f), pointLightColor * glm::vec3(0.9f), glm::vec3(1.0f));
    PointLight pointLight(pointColor, glm::vec3(1.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f);

    Shader* modelShader = ResourceManager::Instance()->getShader("model");
    modelShader->bind();
    // Step projection uniforms
    modelShader->setUniformMat4f("model", model);
    modelShader->setUniformMat4f("view", view);
    modelShader->setUniformMat4f("projection", projection);

    // Step material uniforms, other parameters like diffuse color,
    // diffuse and specular maps are set in Model class
    modelShader->setUniform1f("material.shininess", 32.0f);

    // Step light uniforms
    modelShader->setDirectionalLightUniforms("dirLight", dirLight);
    modelShader->setPointLightUniforms("pointLight", pointLight);
    modelShader->setSpotLightUniforms("spotLight", spotLight);

    // For lighting calculations
    modelShader->setUniformVec3("viewPosition", camera.getPosition());

    // NOTE: diabled for now until we implement ModelRenderer
    model3D->draw(*modelShader);*/

    debugRenderer.drawBox3D(glm::vec3(0.0f, 2.5f, -12.0f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec3(10.0f, 5.0f, 0.1f));
    debugRenderer.drawBox3D(glm::vec3(5.0f, 2.5f, -5.0f), glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), glm::vec3(0.1f, 5.0f, 10.0f));
    debugRenderer.drawBox(glm::vec4(0.0f, 0.0f, 2.0f, 2.0f), -5.0f, glm::vec4(1.0f), 0.0f);

    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);

    skybox->render(projection, view);
}
