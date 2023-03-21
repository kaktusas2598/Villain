#ifndef __GAME_PLAY_SCREEN__
#define __GAME_PLAY_SCREEN__

#include "IGameScreen.hpp"
#include "box2d/box2d.h"
#include "Camera2D.hpp"
#include "SpriteBatch.hpp"
#include "ResourceManager.hpp"
#include "DebugRenderer.hpp"

#include "Box.hpp"
#include "Player.hpp"

class GamePlayScreen : public Villain::IGameScreen {
    public:
        GamePlayScreen();
        ~GamePlayScreen();

        virtual int getNextScreenID() const override;
        virtual int getPrevScreenID() const override;

        // Called on beggining and end of application
        virtual void build() override;
        virtual void destroy() override;

        // Called on entering/exiting focus
        virtual void onEntry() override;
        virtual void onExit() override;

        // Called in main app loop
        virtual void update(float deltaTime) override;
        virtual void draw(float deltaTime) override;

    private:
        Villain::Shader* textureShader;
        Villain::Shader* light2DShader;
        Villain::Texture* boxTexture;
        Villain::Texture* playerSpriteAtlas;
        Villain::SpriteBatch spriteBatch;
        Villain::Camera2D camera;
        Villain::DebugRenderer debugRenderer;
        bool debugRenderMode = true;

        std::unique_ptr<b2World> world;
        std::vector<Box> boxes;
        Player player;
};

#endif // __GAME_PLAY_SCREEN__
