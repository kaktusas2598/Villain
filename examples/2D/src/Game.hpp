#ifndef __GAME__
#define __GAME__

#include "ResourceManager.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Camera2D.hpp"
#include "Level.hpp"
#include "Timer.hpp"

#include "Bullet.hpp"
#include "Player.hpp"
#include <vector>

class Zombie;

class Game {
    public:
        Game();
        ~Game();

        static void handleEvents();
        static void run();
        // Villain Engine callbacks
        static void preUpdate(float dt);
        static void postUpdate(float dt);
        static void preRender(float dt);
        static void postRender(float dt);
        static void onWindowResize(int newWidth, int newHeight);

    private:
        //NOTE: In example game code, most of Game class members need to be static
        //so they can be used in engine callbacks, not sure if this is good idea, but it works well
        //and this way engine can be used to create all kinds of applications possibly
        static Villain::Sprite* testSprite;
        static Villain::SpriteBatch spriteBatch;
        static Villain::Camera2D camera;
        static Texture* playerSpritesheet;
        static Texture* zombieSpritesheet;
        static std::vector<Bullet> bullets;
        static Villain::Level* level;

        static Villain::Timer colorTimer;

        static Player* player;
        static std::vector<Human*> humans;
        static std::vector<Zombie*> zombies;

};

#endif // __GAME__
