#include "MenuScreen.hpp"
#include "Engine.hpp"
#include "InputManager.hpp"
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
if (Villain::InputManager::Instance()->isKeyPressed(SDLK_ESCAPE)) {
        setScreenState(Villain::ScreenState::EXIT_APPLICATION);
    }
}

void MenuScreen::draw(float deltaTime) {

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // GUI
    struct nk_colorf bg;
    struct nk_context * ctx = mainApplication->getNuklearContext();

    int w = 200, h = 90;
    float x = mainApplication->getScreenWidth()/2.0f - w/2.0f;
    float y = mainApplication->getScreenHeight()/2.0f - h/2.0f;

    if (nk_begin(ctx, "Show", nk_rect(x, y, w, h), NK_WINDOW_BORDER)) {
        //nk_layout_row_static(ctx, 30, 80, 1);
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "PLAY")) {
            setScreenState(Villain::ScreenState::CHANGE_NEXT);
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        //nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "EXIT")) {
            setScreenState(Villain::ScreenState::EXIT_APPLICATION);
        }

    }
    nk_end(ctx);

    glEnable(GL_BLEND);
    // Back to regular blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
