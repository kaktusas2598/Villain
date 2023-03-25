#include "App.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
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
    init("assets/scripts/config.lua");
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
