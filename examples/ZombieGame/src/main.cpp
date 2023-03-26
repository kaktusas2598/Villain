#include "Game.hpp"

int main(int argc, char *argv[]) {

    Game mainGame;

    Villain::Engine engine;

    engine.init(&mainGame, "assets/scripts/config.lua");
    engine.run();

    return 0;
}
