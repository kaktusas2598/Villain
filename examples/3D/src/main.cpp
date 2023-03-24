#include "Game.hpp"
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    std::cout << fs::current_path() << ":\n";
    std::string path = fs::current_path();
    for (const auto & entry : fs::directory_iterator(path))
        std::cout << entry.path() << std::endl;
    //fs::temp_directory_path()
    Game mainGame;
    mainGame.run();

    return 0;
}
