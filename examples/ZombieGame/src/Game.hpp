#ifndef __GAME__
#define __GAME__

#include "Engine.hpp"
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

class Game : public Villain::Engine {
    public:
        Game();
        ~Game();

        void handleEvents();
        //static void run();

        void onAppPreUpdate(float deltaTime);
        void onAppPostUpdate(float deltaTime);
        void onAppRender(float deltaTime);
        void onAppWindowResize(int newWidth, int newHeight);

    private:
        void addBlood(const glm::vec2& pos, int numParticles);

        Villain::Sprite* testSprite;
        Villain::SpriteBatch spriteBatch;
        Villain::Camera2D camera;
        Villain::Camera2D hudCamera;
        Texture* playerSpritesheet;
        Texture* zombieSpritesheet;
        std::vector<Bullet> bullets;
        Villain::Level* level;

        Villain::Timer colorTimer;

        Player* player;
        std::vector<Human*> humans;
        std::vector<Zombie*> zombies;

        int numHumansKilled = 0;
        int numZombiesKilled = 0;

        Villain::SpriteBatch textBatch;
        Villain::SpriteFont* spriteFont;

        Villain::FreeType* freeType;

        Villain::ParticleEngine2D particleEngine;
        Villain::ParticleBatch2D* bloodParticles;
};

#endif // __GAME__
