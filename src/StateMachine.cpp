#include "StateMachine.hpp"

#include "IGameScreen.hpp"

namespace Villain {

    StateMachine::StateMachine (Engine* game): mainApplication(game) {}

    StateMachine::~StateMachine () {
        destroy();
    }

    IGameScreen* StateMachine::moveNext() {
        IGameScreen* currentScreen = getCurrentScreen();
        if (currentScreen->getNextScreenID() != STATE_NONE) {
            currentScreenID = currentScreen->getNextScreenID();
        }
        return getCurrentScreen();
    }

    IGameScreen* StateMachine::movePrev() {
        IGameScreen* currentScreen = getCurrentScreen();
        if (currentScreen->getPrevScreenID() != STATE_NONE) {
            currentScreenID = currentScreen->getPrevScreenID();
        }
        return getCurrentScreen();
    }

    void StateMachine::setScreen(int nextScreenID) {
        currentScreenID = nextScreenID;
    }

    void StateMachine::addScreen(IGameScreen* newScreen) {
        newScreen->screenID = screens.size();
        screens.push_back(newScreen);
        newScreen->build();
        newScreen->setParentGame(mainApplication);
    }

    void StateMachine::destroy() {
        for (std::size_t i = 0; i < screens.size(); i++) {
           screens[i]->destroy();
        }
        screens.resize(0);
        currentScreenID = STATE_NONE;
    }

    IGameScreen* StateMachine::getCurrentScreen() {
        if (currentScreenID == STATE_NONE) {
            return nullptr;
        }

        return screens[currentScreenID];
    }
}

