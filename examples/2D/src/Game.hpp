#ifndef __GAME__
#define __GAME__

#include "ResourceManager.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Camera2D.hpp"

#include "Bullet.hpp"

class Game {
    public:
        Game();
        ~Game();

        static void handleEvents();
        void run();

        // Villain Engine callbacks
        static void preUpdate(float dt);
        static void postUpdate(float dt);
        static void preRender(float dt);
        static void postRender(float dt);
        static void onWindowResize(int newWidth, int newHeight);

    private:
        static Villain::Sprite* testSprite;
        static Villain::SpriteBatch spriteBatch;
        static Villain::Camera2D camera;
        static Texture* playerSpritesheet;
        static std::vector<Bullet> bullets;

};

#endif // __GAME__
