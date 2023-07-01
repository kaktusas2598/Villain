#ifndef __GAME__
#define __GAME__

#include "Application.hpp"
#include "ResourceManager.hpp"
#include "rendering/SpriteFont.hpp"
#include "rendering/SpriteBatch.hpp"
#include "tiled/Level.hpp"
#include "Timer.hpp"
#include "ParticleEngine2D.hpp"
#include "ParticleBatch2D.hpp"

#include "Bullet.hpp"
#include "Player.hpp"
#include "FreeType.hpp"
#include <vector>

class Zombie;

class Game : public Villain::Application {
    public:
        ~Game();

        void init();
        void addStates() {};
        void handleEvents();

        void onAppPreUpdate(float deltaTime);
        void onAppPostUpdate(float deltaTime);
        void onAppRender(float deltaTime);
        void onAppWindowResize(int newWidth, int newHeight);

    private:
        void addBlood(const glm::vec2& pos, int numParticles);

        Villain::SpriteBatch spriteBatch;
        Villain::Camera* camera;
        Villain::Camera* hudCamera;
        Villain::Texture* playerSpritesheet;
        Villain::Texture* zombieSpritesheet;
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
