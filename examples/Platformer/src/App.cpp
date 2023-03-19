#include "App.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "LuaScript.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "DebugConsole.hpp"
#include <cmath>
#include <cstdio>
#include <random>
#include <sstream>


using namespace Villain;

App::App() {
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
}

void App::onInit() {

}

void App::addStates() {
    gameScreen = std::make_unique<GamePlayScreen>();
    stateMachine->addScreen(gameScreen.get());
    stateMachine->setScreen(gameScreen->getID());
}

void App::onExit() {

}


App::~App() {
    SoundManager::Instance()->cleanSoundMaps();
    //ResourceManager::Instance()->clearTextureMap();
}

void App::handleEvents() {
    //if(TheInputManager::Instance()->isKeyDown(SDLK_q))
        //camera.setZoom(camera.getZoom() + 0.01f);
    //if(TheInputManager::Instance()->isKeyDown(SDLK_e))
        //camera.setZoom(camera.getZoom() - 0.01f);

    //// Get SDL window mouse coords and convert to camera woorld coords
    //glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
    //mouseCoords = camera.screenToWorld(mouseCoords);

    //// Format message and add it in debug console
    //std::ostringstream ss;
    //ss << "Mouse world position: " << mouseCoords.x << ", " << mouseCoords.y;
    //DebugConsole::Instance()->setInfo("mouse", ss.str());
}

void App::onAppPreUpdate(float dt) {
    handleEvents();
}

void App::onAppPostUpdate(float dt) {
}

void App::onAppRender(float dt) {

    //Shader* textShader = ResourceManager::Instance()->getShader("textBatch");
    //if (textShader != nullptr) {
        //glm::vec4 color(0.0f, 0.2f, 1.0f, 1.0f);
        //textShader->bind();
        //textShader->setUniformMat4f("view", hudCamera.getCameraMatrix());
        //textShader->setUniformMat4f("projection", projection);
        //textShader->setUniform1i("spriteTexture", 0);

        //textBatch.begin();

        //spriteFont->draw(textBatch, "TESTING", hudCamera.screenToWorld(glm::vec2(10.0f, 150.0f)), glm::vec2(3.0f), 0.6f, color);
        //std::stringstream ss;
        //ss << "HP: " << (int)player->getHealth();
        //freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 70.0f)), 2.0f, 0.6f, color);
        //ss.str(""); // Empty string stream

        //ss << "Humans: " << humans.size();
        //freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 10.0f)), 2.0f, 0.6f, color);
        //ss.str(""); // Empty string stream

        //ss << "Zombies: " << zombies.size();
        //freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 40.0f)), 2.0f, 0.6f, color);

        //textBatch.end();

        //textBatch.renderBatch();
    //}
}

void App::onAppWindowResize(int newWidth, int newHeight) {
   //camera.init(newWidth, newHeight);
   //glm::vec3 camPos = camera.getPosition();
   //camPos.x = newWidth/2.0;
   //camPos.y = newHeight/2.0;
   //camera.setPosition(camPos);
}
