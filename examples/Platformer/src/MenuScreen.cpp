#include "MenuScreen.hpp"
#include "Engine.hpp"
#include "Input.hpp"
#include "glm/glm.hpp"
#include "nuklear.h"

MenuScreen::MenuScreen() {}

MenuScreen::~MenuScreen() {}

int MenuScreen::getNextScreenID() const {
    return 1;
}


int MenuScreen::getPrevScreenID() const {
    return 1;
}

void MenuScreen::build() {

}

void MenuScreen::destroy() {

}

void MenuScreen::onEntry() {
}

void MenuScreen::onExit() {
}

void MenuScreen::update(float deltaTime) {
if (Villain::Input::Get()->isKeyPressed(SDLK_ESCAPE)) {
        setScreenState(Villain::ScreenState::EXIT_APPLICATION);
    }
}

void MenuScreen::draw() {
    auto uiManager = mainApplication->getEngine()->getUIManager();
    int w = 200, h = 90;
    float x = Villain::Engine::getScreenWidth()/2.0f - w/2.0f;
    float y = Villain::Engine::getScreenHeight()/2.0f - h/2.0f;

    if (uiManager.beginWindow("Show", x, y, w, h)) {
        uiManager.layoutRowDynamic(30, 1);
        if (uiManager.button("PLAY")) {
            setScreenState(Villain::ScreenState::CHANGE_NEXT);
        }

        uiManager.layoutRowDynamic(30, 1);
        if (uiManager.button("EXIT")) {
            setScreenState(Villain::ScreenState::EXIT_APPLICATION);
        }
    }
    uiManager.endWindow();

    //glEnable(GL_BLEND);
    // Back to regular blending
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

