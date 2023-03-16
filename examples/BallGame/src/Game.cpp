#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "LuaScript.hpp"
#include "ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "DebugConsole.hpp"
#include <cmath>
#include <cstdio>
#include <memory>
#include <random>
#include <sstream>

#include "SoundManager.hpp"

struct BallSpawn {
    BallSpawn(const glm::vec4 colr, float rad, float m, float minSpeed, float maxSpeed, float prob) :
        color(colr),
        radius(rad),
        mass(m),
        randSpeed(minSpeed, maxSpeed),
        probability(prob)
    {}

    glm::vec4 color;
    float radius;
    float mass;
    float probability;
    std::uniform_real_distribution<float> randSpeed;
};

using namespace Villain;

Game::Game() {
    // ENGINE INIT
    LuaScript configScript("assets/scripts/config.lua");
    configScript.open();

    unsigned int flags = 0;
    if (configScript.get<bool>("window.fullscreen"))
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (configScript.get<bool>("window.borderless"))
        flags |= SDL_WINDOW_BORDERLESS;
    if (configScript.get<bool>("window.resizable"))
        flags |= SDL_WINDOW_RESIZABLE;

    init(
            configScript.get<std::string>("window.title"),
            configScript.get<int>("window.height"),
            configScript.get<int>("window.width"),
            flags
            );

    // RESOURCES INIT
    camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    glm::vec3 camPos = camera.getPosition();
    camPos.x = configScript.get<int>("window.width")/2.0;
    camPos.y = configScript.get<int>("window.height")/2.0;
    camera.setPosition(camPos);

    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "batch");
    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/text.frag", "textBatch");

    spriteBatch.init();
    textBatch.init();

    freeType = new FreeType("assets/fonts/PixelEmulator.ttf", 16);

    // GAME INIT
    ballRenderers.push_back(std::make_unique<BallRenderer>());

    initBalls();
}

Game::~Game() {
    SoundManager::Instance()->cleanSoundMaps();
    //ResourceManager::Instance()->clearTextureMap();
}

void Game::handleEvents() {
    if(TheInputManager::Instance()->isKeyDown(SDLK_q))
        camera.setZoom(camera.getZoom() + 0.01f);
    if(TheInputManager::Instance()->isKeyDown(SDLK_e))
        camera.setZoom(camera.getZoom() - 0.01f);

    // Get SDL window mouse coords and convert to camera woorld coords
    glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
    mouseCoords = camera.screenToWorld(mouseCoords);

    // Format message and add it in debug console
    std::ostringstream ss;
    ss << "Mouse world position: " << mouseCoords.x << ", " << mouseCoords.y;
    DebugConsole::Instance()->setInfo("mouse", ss.str());

    if (InputManager::Instance()->isKeyDown(SDLK_LEFT)) {
        ballController.setGravityDirection(GravityDirection::LEFT);
    } else if (InputManager::Instance()->isKeyDown(SDLK_RIGHT)) {
        ballController.setGravityDirection(GravityDirection::RIGHT);
    } else if (InputManager::Instance()->isKeyDown(SDLK_UP)) {
        ballController.setGravityDirection(GravityDirection::UP);
    } else if (InputManager::Instance()->isKeyDown(SDLK_DOWN)) {
        ballController.setGravityDirection(GravityDirection::UP);
    }
}

void Game::initBalls() {
    const int NUM_BALLS = 100;

    std::mt19937 rndEngine((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> randX(0.0f, getScreenWidth());
    std::uniform_real_distribution<float> randY(0.0f, getScreenHeight());
    std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

    std::vector<BallSpawn> possibleBalls;
    float totalProbability = 0.0f;

    totalProbability += 20.0f;
    possibleBalls.emplace_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 20.f, 1.f, 0.1f, 7.0f, totalProbability);

    totalProbability += 10.0f;
    possibleBalls.emplace_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 30.f, 2.f, 0.1f, 3.0f, totalProbability);

    totalProbability += 1.0f;
    possibleBalls.emplace_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 50.f, 4.f, 0.0f, 0.0f, totalProbability);

    std::uniform_real_distribution<float> spawn(0.0f, totalProbability);

    balls.reserve(NUM_BALLS);

    BallSpawn* ballToSpawn = &possibleBalls[0];
    for (int i = 0; i < NUM_BALLS; i++) {
        float spawnVal = spawn(rndEngine);
        for (auto i = 0; i < possibleBalls.size(); i++) {
            if (spawnVal <= possibleBalls[i].probability) {
                ballToSpawn = &possibleBalls[i];
                break;
            }
        }

        // random starting pos
        glm::vec2 pos(randX(rndEngine), randY(rndEngine));

        // random dir, hacky
        glm::vec2 direction(randDir(rndEngine), randDir(rndEngine));
        if (direction.x != 0.0f || direction.y != 0.0f) {
            direction = glm::normalize(direction);
        } else {
            direction = glm::vec2(1.0f, 0.0f); // default direction in case dir is 0
        }

        balls.emplace_back(ballToSpawn->radius, ballToSpawn->mass,
                pos, direction * ballToSpawn->randSpeed(rndEngine),
                ResourceManager::Instance()->loadTexture("assets/textures/circle.png", "circle")->getID(),
                ballToSpawn->color);
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents();
}

void Game::onAppPostUpdate(float dt) {
    ballController.updateBalls(balls, dt, getScreenWidth(), getScreenHeight());
}

void Game::onAppRender(float dt) {

    // Bind texture
    ResourceManager::Instance()->getTexture("circle")->bind();
    // Set uniforms
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.2f));
    model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    //glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 projection = glm::ortho(0.0f, (float)TheEngine::Instance()->getScreenWidth(), 0.0f, (float)TheEngine::Instance()->getScreenHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    float colorMin = 0.0f, colorMax = 255.0f, colorCurr = colorMin;
    colorCurr+= (int)colorTimer.read() % 255;
    if (colorCurr > colorMax) {
        colorCurr = colorMin;
    }

    // Setting up rendering batch and rendering it all at once with a single draw call
    Shader* batchShader = ResourceManager::Instance()->getShader("batch");
    if (batchShader != nullptr) {
        batchShader->bind();
        //spriteShader->setUniformMat4f("model", model);
        batchShader->setUniformMat4f("view", view);
        batchShader->setUniformMat4f("projection", projection);
        batchShader->setUniform1i("spriteTexture", 0);
        spriteBatch.begin();

        for (int i = 0; i < balls.size(); i++) {
            std::cout << i << "= Position: " << balls[i].position.x << ", " << balls[i].position.y << "\n";
            ballRenderers[currentRenderer]->renderBall(spriteBatch, balls[i]);
        }

        spriteBatch.end();

        spriteBatch.renderBatch();
    }

    Shader* textShader = ResourceManager::Instance()->getShader("textBatch");
    if (textShader != nullptr) {
        glm::vec4 color(0.0f, 0.2f, 1.0f, 1.0f);
        textShader->bind();
        //textShader->setUniformMat4f("view", hudCamera.getCameraMatrix());
        textShader->setUniformMat4f("projection", projection);
        textShader->setUniform1i("spriteTexture", 0);

        textBatch.begin();

        std::stringstream ss;
        ss << "FPS: " << getFps();
        freeType->draw(textBatch, ss.str(), glm::vec2(10.0f, 10.0f), 2.0f, 0.6f, color);

        textBatch.end();

        textBatch.renderBatch();
    }
}

void Game::onAppWindowResize(int newWidth, int newHeight) {
   camera.init(newWidth, newHeight);
}
