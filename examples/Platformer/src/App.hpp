#ifndef __APP__
#define __APP__

#include "Engine.hpp"
#include "GamePlayScreen.hpp"
#include "MenuScreen.hpp"

class Zombie;

class App : public Villain::Application {
    public:
        ~App();
        void init() override;
        void addStates() override;
        void onAppWindowResize(int newWidth, int newHeight) override;
    private:
        std::unique_ptr<GamePlayScreen> gameScreen = nullptr;
        std::unique_ptr<MenuScreen> menuScreen = nullptr;
};

#endif // __APP__
