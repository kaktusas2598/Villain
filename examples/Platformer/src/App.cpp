#include "App.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

using namespace Villain;

void App::init() {

}

void App::addStates() {
    menuScreen = std::make_unique<MenuScreen>();
    gameScreen = std::make_unique<GamePlayScreen>();

    stateMachine->addScreen(menuScreen.get());
    stateMachine->addScreen(gameScreen.get());

    stateMachine->setScreen(menuScreen->getID());
}

App::~App() {}

void App::onAppWindowResize(int newWidth, int newHeight) {
    if (stateMachine->getCurrentScreen()->getID() == gameScreen->getID())
        gameScreen->onAppWindowResize(newWidth, newHeight);
}
