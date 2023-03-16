#ifndef __GAME__
#define __GAME__

#include "ResourceManager.hpp"
#include "SpriteFont.hpp"
#include "Sprite.hpp"
#include "SpriteBatch.hpp"
#include "Camera2D.hpp"
#include "Level.hpp"
#include "Timer.hpp"
#include "ParticleEngine2D.hpp"
#include "ParticleBatch2D.hpp"

#include "Bullet.hpp"
#include "Player.hpp"
#include "FreeType.hpp"
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
        static void addBlood(const glm::vec2& pos, int numParticles);
        //NOTE: In example game code, most of Game class members need to be static
        //so they can be used in engine callbacks, not sure if this is good idea, but it works well
        //and this way engine can be used to create all kinds of applications possibly
        static Villain::Sprite* testSprite;
        static Villain::SpriteBatch spriteBatch;
        static Villain::Camera2D camera;
        static Villain::Camera2D hudCamera;
        static Texture* playerSpritesheet;
        static Texture* zombieSpritesheet;
        static std::vector<Bullet> bullets;
        static Villain::Level* level;

        static Villain::Timer colorTimer;

        static Player* player;
        static std::vector<Human*> humans;
        static std::vector<Zombie*> zombies;

        static int numHumansKilled;
        static int numZombiesKilled;

        static Villain::SpriteBatch textBatch;
        static Villain::SpriteFont* spriteFont;

        static Villain::FreeType* freeType;

        static Villain::ParticleEngine2D particleEngine;
        static Villain::ParticleBatch2D* bloodParticles;

};

#endif // __GAME__
