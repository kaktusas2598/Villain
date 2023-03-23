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
    menuScreen = std::make_unique<MenuScreen>();
    gameScreen = std::make_unique<GamePlayScreen>();

    stateMachine->addScreen(menuScreen.get());
    stateMachine->addScreen(gameScreen.get());

    stateMachine->setScreen(menuScreen->getID());
}

void App::onExit() {

}


App::~App() {
    SoundManager::Instance()->cleanSoundMaps();
    //ResourceManager::Instance()->clearTextureMap();
}

void App::handleEvents() {
}

void App::onAppPreUpdate(float dt) {
    handleEvents();
}

void App::onAppPostUpdate(float dt) {
}

void App::onAppRender(float dt) {
}

void App::onAppWindowResize(int newWidth, int newHeight) {
    if (stateMachine->getCurrentScreen()->getID() == gameScreen->getID())
        gameScreen->onAppWindowResize(newWidth, newHeight);
}
