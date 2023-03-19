#ifndef __APP__
#define __APP__

#include "Engine.hpp"
#include "GamePlayScreen.hpp"
#include "ResourceManager.hpp"
#include "SpriteFont.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Camera2D.hpp"
#include "Level.hpp"
#include "Timer.hpp"

#include "FreeType.hpp"
#include <memory>
#include <vector>

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

        Villain::SpriteBatch spriteBatch;
        Villain::Camera2D camera;
        Villain::Camera2D hudCamera;
        Villain::Level* level;
        Villain::Timer colorTimer;
        Villain::SpriteBatch textBatch;
        Villain::SpriteFont* spriteFont;
        Villain::FreeType* freeType;
};

#endif // __APP__
