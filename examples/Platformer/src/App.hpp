#ifndef __APP__
#define __APP__

#include "Engine.hpp"
#include "GamePlayScreen.hpp"
#include "MenuScreen.hpp"

class Zombie;

class App : public Villain::Engine {
    public:
        App();
        ~App();

        void onInit() override;
        void addStates() override;
        void onExit() override;
        void handleEvents();

        void onAppPreUpdate(float deltaTime) override;
        void onAppPostUpdate(float deltaTime) override;
        void onAppRender(float deltaTime) override;
        void onAppWindowResize(int newWidth, int newHeight) override;

    private:
        std::unique_ptr<GamePlayScreen> gameScreen = nullptr;
        std::unique_ptr<MenuScreen> menuScreen = nullptr;
};

#endif // __APP__
