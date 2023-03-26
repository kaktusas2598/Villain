#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "Light.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "DebugConsole.hpp"
#include "DebugRenderer.hpp"
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <random>
#include <sstream>

using namespace Villain;

void Game::init() {
    model3D = new Model("assets/models/donut.obj");

    //camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    //glm::vec3 camPos = camera.getPosition();
    //camPos.x = configScript.get<int>("window.width")/2.0;
    //camPos.y = configScript.get<int>("window.width")/2.0;
    //camera.setPosition(camPos);

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
}

Game::~Game() {
    debugRenderer.dispose();
}

void Game::handleEvents(float deltaTime) {
    // Get SDL window mouse coords and convert to camera woorld coords
    glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
    //mouseCoords = camera.screenToWorld(mouseCoords);

    // Format message and add it in debug console
    //std::ostringstream ss;
    //ss << "Mouse world position: " << mouseCoords.x << ", " << mouseCoords.y;
    //DebugConsole::Instance()->setInfo("mouse", ss.str());

    static bool firstMouse = true;
    static float lastX = Engine::getScreenWidth() / 2.0f;
    static float lastY = Engine::getScreenHeight() / 2.0f;
    // To prevent sudden camera jamp when mouse callback first gets called upon entering screen
    if (firstMouse) {
        lastX = mouseCoords.x;
        lastY = mouseCoords.y;
        firstMouse = false;
    }

    float xOffset = mouseCoords.x - lastX;
    float yOffset = lastY - mouseCoords.y; // Reversed because Y axis goes from bottom to top in opengl

    lastX = mouseCoords.x;
    lastY = mouseCoords.y;
    //camera.processMouseMovement(xOffset, yOffset);


    //FIXME: relative mode fixes camera restraint problem, but camera never stops moving for some reason
    if (!Engine::editModeActive()) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), 0, 0); // this seem to fix moving camera issue, but cursor disappears?
    }
    glm::vec2 mouseOffsets = TheInputManager::Instance()->getMouseOffsets();
    camera.processMouseMovement(mouseOffsets.x, mouseOffsets.y);

    // TODO:
    //camera.processMouseScroll(yOffset);

    if (InputManager::Instance()->isKeyDown(SDLK_w)) {
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_s)) {
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_a)) {
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    }
    if (InputManager::Instance()->isKeyDown(SDLK_d)) {
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    }

    // Camera up and down, same controls as in minecraft creative
    if (InputManager::Instance()->isKeyDown(SDLK_SPACE)) {
            camera.Position.y += 2.5f * deltaTime;
    }
    if (InputManager::Instance()->isKeyDown(SDLK_LSHIFT)) {
            camera.Position.y -= 2.5f * deltaTime;
    }

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
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -5.0f));
    //model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(4.0f));
    glm::mat4 view = camera.getViewMatrix();

    // First param - FOV could be changed for zooming effect
    // 2nd param - aspect ratio
    // 3rd and 4th params - near and far planes
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Engine::getScreenWidth()/(float)Engine::getScreenHeight(), 0.1f, 100.0f);

    LightColor dirColor(glm::vec3(0.5f), glm::vec3(0.2f), glm::vec3(1.0f));
    DirectionalLight dirLight(dirColor, glm::vec3(-0.2f, -1.0f, -0.3f));

    glm::vec3 spotLightColor = glm::vec3(1.0f, 1.0f, 0.0f);
    LightColor spotColor(spotLightColor * glm::vec3(0.2f), spotLightColor * glm::vec3(0.9f), glm::vec3(1.0f));
    // Make spot light position same as camera thus simulating flashlight!
    SpotLight spotLight(spotColor, camera.Position, camera.Front, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

    glm::vec3 pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
    LightColor pointColor(pointLightColor * glm::vec3(0.5f), pointLightColor * glm::vec3(0.9f), glm::vec3(1.0f));
    float constant = 1.0f;
    float linear = 0.022f;
    float quadratic = 0.0019f;
    PointLight pointLight(pointColor, glm::vec3(1.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f);

    Shader* modelShader = ResourceManager::Instance()->getShader("model");
    if (modelShader != nullptr) {
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
        modelShader->setUniformVec3("viewPosition", camera.Position);

        model3D->draw(*modelShader);
    }

    debugRenderer.drawBox3D(glm::vec3(0.0f, 2.5f, -12.0f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), glm::vec3(10.0f, 5.0f, 0.1f));
    debugRenderer.drawBox3D(glm::vec3(5.0f, 2.5f, -5.0f), glm::vec4(0.1f, 0.9f, 0.1f, 1.0f), glm::vec3(0.1f, 5.0f, 10.0f));
    debugRenderer.drawBox(glm::vec4(0.0f, 0.0f, 2.0f, 2.0f), -5.0f, glm::vec4(1.0f), 0.0f);

    debugRenderer.end();
    debugRenderer.render(projection * view, 2.0f);

    skybox->render(projection, view);
}

void Game::onAppWindowResize(int newWidth, int newHeight) {
    //camera.init(newWidth, newHeight);
}

