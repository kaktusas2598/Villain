#include "App.hpp"

int main(int argc, char *argv[]) {

    App mainGame;
    Villain::Engine engine;
    engine.init(&mainGame, "assets/scripts/config.lua");
    engine.run();

    return 0;
}
