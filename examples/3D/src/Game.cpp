#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "LuaScript.hpp"
#include "Model.hpp"
#include "ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "DebugConsole.hpp"
#include <cstdio>
#include <random>
#include <sstream>

using namespace Villain;

Game::Game() {
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

    model = new Model("assets/models/donut.obj");

    //camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    //glm::vec3 camPos = camera.getPosition();
    //camPos.x = configScript.get<int>("window.width")/2.0;
    //camPos.y = configScript.get<int>("window.width")/2.0;
    //camera.setPosition(camPos);

    //ResourceManager::Instance()->loadShader("assets/shaders/model.vert", "assets/shaders/model.frag", "model");
    ResourceManager::Instance()->loadShader("assets/shaders/models.glsl", "model");

    //NOTE: So far not sure if sprite class will be used, due to sprite batch existance


    // Randomise some NPCs
    //std::mt19937 rndEngine;
    //rndEngine.seed(time(nullptr));
    //std::uniform_real_distribution<float> xDist(100.0f, level->getWidth() - 100.0f);
    //std::uniform_real_distribution<float> yDist(100.0f, level->getHeight() - 100.0f);

}

Game::~Game() {

}

void Game::handleEvents() {
    //if(TheInputManager::Instance()->isKeyDown(SDLK_q))
        //camera.setZoom(camera.getZoom() + 0.01f);
    //if(TheInputManager::Instance()->isKeyDown(SDLK_e))
        //camera.setZoom(camera.getZoom() - 0.01f);

    // Get SDL window mouse coords and convert to camera woorld coords
    glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
    //mouseCoords = camera.screenToWorld(mouseCoords);

    // Format message and add it in debug console
    //std::ostringstream ss;
    //ss << "Mouse world position: " << mouseCoords.x << ", " << mouseCoords.y;
    //DebugConsole::Instance()->setInfo("mouse", ss.str());
}

void Game::onAppPreUpdate(float dt) {
    handleEvents();
}

void Game::onAppPostUpdate(float dt) {
}

void Game::onAppRender(float dt) {
    // Bind texture
    // Set uniforms
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.2f));
    model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    //glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 projection = glm::ortho(0.0f, (float)TheEngine::Instance()->getScreenWidth(), 0.0f, (float)TheEngine::Instance()->getScreenHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    Shader* modelShader = ResourceManager::Instance()->getShader("model");
    if (modelShader != nullptr) {
        //spriteShader->bind();
        //spriteShader->setUniformMat4f("model", model);
        //spriteShader->setUniformMat4f("view", view);
        //spriteShader->setUniformMat4f("projection", projection);
        //spriteShader->setUniform1i("spriteTexture", 0);
        ////spriteShader->setUniformVec4("uColor", glm::vec4(1.0f, (float)(colorTimer.read() / 255.0f), 0.0f, 1.0f));
        //spriteShader->setUniformVec4("uColor", glm::vec4(1.0f, colorCurr, 0.0f, 1.0f));
        ////spriteShader->setUniformVec4("uColor", glm::vec4(0.5f, 1.0f, 0.2f, 1.0f));
        //model->draw(*modelShader);
    }

    // Setting up rendering batch and rendering it all at once with a single draw call
    //Shader* batchShader = ResourceManager::Instance()->getShader("batch");
    //if (batchShader != nullptr) {
        //batchShader->bind();
        ////spriteShader->setUniformMat4f("model", model);
        //batchShader->setUniformMat4f("view", view);
        //batchShader->setUniformMat4f("projection", projection);
        //batchShader->setUniform1i("spriteTexture", 0);
        //spriteBatch.begin();

        //glm::vec4 position(0.0f, 0.0f, 50.0f, 50.0f);
        ////glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
        //glm::vec4 uv(0.0f, 0.0f, 1.0f / 6, 1.0f / 10);
        //glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        ////testBatch->draw(position, uv, playerSpritesheet->getID(), 0.0f, color);
        //spriteBatch.draw(position + glm::vec4(100.0f, 0.0f, 0.0f, 0.0f), uv, playerSpritesheet->getID(), 0.5f, color);

        //static int currentFrame = 0;
        //static int numFrames = 6;
        //currentFrame =  int(((SDL_GetTicks() / 100) % 5));
        //spriteBatch.draw(position, currentFrame++, 2, 48, 48, playerSpritesheet, 0.5f, color);

        //spriteBatch.draw(position + glm::vec4(50.0f, 0.0f, 0.0f, 0.0f), currentFrame, 0, 48, 48, playerSpritesheet, 0.5f, color);

        //spriteBatch.end();

        //spriteBatch.renderBatch();
    //}
}

void Game::onAppWindowResize(int newWidth, int newHeight) {
   //camera.init(newWidth, newHeight);
}

