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
    hudCamera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    glm::vec3 camPos = camera.getPosition();
    camPos.x = configScript.get<int>("window.width")/2.0;
    camPos.y = configScript.get<int>("window.height")/2.0;
    camera.setPosition(camPos);

    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "batch");
    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/text.frag", "textBatch");

    spriteBatch.init();
    textBatch.init();

    freeType = new FreeType("assets/fonts/PixelEmulator.ttf", 16);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

    if (InputManager::Instance()->isKeyPressed(SDLK_LEFT)) {
        ballController.setGravityDirection(GravityDirection::LEFT);
    } else if (InputManager::Instance()->isKeyPressed(SDLK_RIGHT)) {
        ballController.setGravityDirection(GravityDirection::RIGHT);
    } else if (InputManager::Instance()->isKeyPressed(SDLK_UP)) {
        ballController.setGravityDirection(GravityDirection::UP);
    } else if (InputManager::Instance()->isKeyPressed(SDLK_DOWN)) {
        ballController.setGravityDirection(GravityDirection::DOWN);
    }
}

void Game::initBalls() {


    // Initialise the spatial partition grid
    // NOTE:will need to be recalculated on window resize
    grid = std::make_unique<Grid>(getScreenWidth(), getScreenHeight(), CELL_SIZE);

#define ADD_BALL(p, ...) \
    totalProbability += p; \
    possibleBalls.emplace_back(__VA_ARGS__);


    // 1250 max without spatial partitioning to reach 60FPS
    const int NUM_BALLS = 10000;

    std::mt19937 rndEngine((unsigned int)time(nullptr));
    std::cout << getScreenWidth() << ", " << getScreenHeight() << "\n";
    std::uniform_real_distribution<float> randX(0.0f, getScreenWidth());
    std::uniform_real_distribution<float> randY(0.0f, getScreenHeight());
    std::uniform_real_distribution<float> randDir(-1.0f, 1.0f);

    std::vector<BallSpawn> possibleBalls;
    float totalProbability = 0.0f;

    ADD_BALL(20.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.f, 1.f, 0.1f, 7.0f, totalProbability)
    ADD_BALL(10.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 3.f, 2.f, 0.1f, 3.0f, totalProbability)
    ADD_BALL(1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 5.f, 4.f, 0.0f, 0.0f, totalProbability)

    std::uniform_real_distribution<float> spawn(0.0f, totalProbability);

    balls.reserve(NUM_BALLS);

    BallSpawn* ballToSpawn = &possibleBalls[0];
    for (int i = 0; i < NUM_BALLS; i++) {
        float spawnVal = spawn(rndEngine);
        for (auto j = 0; j < possibleBalls.size(); j++) {
            if (spawnVal <= possibleBalls[j].probability) {
                ballToSpawn = &possibleBalls[j];
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
        // Avoid dangling pointers by not calling emplace_back on balls vector after init
        grid->addBall(&balls.back());
    }
}

void Game::onAppPreUpdate(float dt) {
    handleEvents();
}

void Game::onAppPostUpdate(float dt) {
    ballController.updateBalls(balls, grid.get(), dt, getScreenWidth(), getScreenHeight());
    camera.update();
}

void Game::onAppRender(float dt) {

    //glm::mat4 projection = glm::ortho(0.0f, (float)getScreenWidth(), 0.0f, (float)getScreenHeight(), 0.1f, 100.0f);
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
            //std::cout << i << "= Position: " << balls[i].position.x << ", " << balls[i].position.y << "\n";
            ballRenderers[currentRenderer]->renderBall(spriteBatch, balls[i]);
        }

        spriteBatch.end();

        spriteBatch.renderBatch();
    }

    Shader* textShader = ResourceManager::Instance()->getShader("textBatch");
    if (textShader != nullptr) {
        glm::vec4 color(0.0f, 0.2f, 1.0f, 1.0f);
        textShader->bind();
        textShader->setUniformMat4f("view", hudCamera.getCameraMatrix());
        textShader->setUniformMat4f("projection", projection);
        textShader->setUniform1i("spriteTexture", 0);

        textBatch.begin();

        std::stringstream ss;
        ss << "FPS: " << (int) getFps();
        freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 10.0f)), 2.0f, 0.6f, color);

        textBatch.end();

        textBatch.renderBatch();
    }
}

void Game::onMouseMove(int mouseX, int mouseY) {
    ballController.onMouseMove(balls, (float)mouseX, (float)getScreenHeight() - (float)mouseY);
}

void Game::onMouseDown(int mouseX, int mouseY) {
    ballController.onMouseDown(balls, (float)mouseX, (float)getScreenHeight() - (float)mouseY);
}

void Game::onMouseUp() {
    ballController.onMouseUp(balls);
}


void Game::onAppWindowResize(int newWidth, int newHeight) {
    std::cout << "Window resize event, new size: " << newWidth << ", " << newHeight << "\n";
    camera.init(newWidth, newHeight);
    glm::vec3 camPos = camera.getPosition();
    camPos.x = newWidth/2.0;
    camPos.y = newHeight/2.0;
    camera.setPosition(camPos);

    //grid->resize(newWidth, newHeight);
}
