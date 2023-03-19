#include "GamePlayScreen.hpp"

GamePlayScreen::GamePlayScreen() {}

GamePlayScreen::~GamePlayScreen() {}

int GamePlayScreen::getNextScreenID() const {
    return STATE_NONE;
}


int GamePlayScreen::getPrevScreenID() const {
    return STATE_NONE;
}

void GamePlayScreen::build() {

}

void GamePlayScreen::destroy() {

}

void GamePlayScreen::onEntry() {

}

void GamePlayScreen::onExit() {

}

void GamePlayScreen::update(float deltaTime) {
    std::cout << "Screen Update!\n";
}

void GamePlayScreen::draw(float deltaTime) {
    std::cout << "Screen Draw!\n";
}

