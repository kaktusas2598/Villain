#include "Game.hpp"

#include "Engine.hpp"
#include "ErrorHandler.hpp"
#include "LevelParser.hpp"
#include "LuaScript.hpp"
#include "ResourceManager.hpp"
#include "examples/2D/src/Player.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "DebugConsole.hpp"
#include <cmath>
#include <cstdio>
#include <random>
#include <sstream>

#include "Gun.hpp"
#include "Zombie.hpp"
#include "SoundManager.hpp"

using namespace Villain;

Sprite* Game::testSprite = nullptr;
SpriteBatch Game::spriteBatch;
Camera2D Game::camera;
Camera2D Game::hudCamera;
Texture* Game::playerSpritesheet = nullptr;
Texture* Game::zombieSpritesheet = nullptr;
std::vector<Bullet> Game::bullets;
Level* Game::level = nullptr;
Timer Game::colorTimer;
SpriteFont* Game::spriteFont = nullptr;
SpriteBatch Game::textBatch;
FreeType* Game::freeType = nullptr;
ParticleEngine2D Game::particleEngine;
ParticleBatch2D* Game::bloodParticles = nullptr;

std::vector<Human*> Game::humans;
std::vector<Zombie*> Game::zombies;
Player* Game::player = nullptr;
int Game::numHumansKilled = 0;
int Game::numZombiesKilled = 0;


const float HUMAN_SPEED = 50.f;
const float ZOMBIE_SPEED = 40.f;

//void updateBlood(Particle2D& p, float dt) {
    //p.position += p.velocity * dt;
    //p.color.a -= p.life * dt;
//}

Game::Game() {
    LuaScript configScript("assets/scripts/config.lua");
    configScript.open();

    unsigned int flags = 0;
    if (configScript.get<bool>("window.fullscreen"))
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (configScript.get<bool>("window.borderless"))
        flags |= SDL_WINDOW_BORDERLESS;
    if (configScript.get<bool>("window.resizable"))
        flags |= SDL_WINDOW_RESIZABLE;

    TheEngine::Instance()->setCallbacks(preUpdate, postUpdate, preRender, postRender);
    TheEngine::Instance()->setWindowCallbacks(onWindowResize);
    TheEngine::Instance()->init(
            configScript.get<std::string>("window.title"),
            configScript.get<int>("window.height"),
            configScript.get<int>("window.width"),
            flags
            );

    SoundManager::Instance()->load("assets/audio/drive.mp3", "drive", SoundType::SOUND_MUSIC);
    SoundManager::Instance()->load("assets/audio/pistol.wav", "pistol", SoundType::SOUND_SFX, -50);
    SoundManager::Instance()->load("assets/audio/cg1.wav", "rifle", SoundType::SOUND_SFX, -60);
    SoundManager::Instance()->load("assets/audio/shotgun.wav", "shotgun", SoundType::SOUND_SFX, -50);
    SoundManager::Instance()->load("assets/audio/zombie.wav", "zombie", SoundType::SOUND_SFX, -20);
    //SoundManager::Instance()->playMusic("drive");

    camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    hudCamera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    glm::vec3 camPos = camera.getPosition();
    camPos.x = configScript.get<int>("window.width")/2.0;
    camPos.y = configScript.get<int>("window.height")/2.0;
    camera.setPosition(camPos);

    bloodParticles = new ParticleBatch2D();
    bloodParticles->init(
            1000,
            0.05f,
            ResourceManager::Instance()->loadTexture("assets/textures/particle.png", "particle"),
            // Lambda
            [] (Particle2D& p, float dt) {
                p.position += p.velocity * dt;
                p.color.a -= p.life * dt * 0.5f;
            }
            // Or normal function: updateBlood
            // Or nothing - default particle update behaviour
            );
    particleEngine.addParticleBatch(bloodParticles);

    playerSpritesheet = ResourceManager::Instance()->loadTexture("assets/textures/player.png", "player");
    zombieSpritesheet = ResourceManager::Instance()->loadTexture("assets/textures/slime.png", "zombie");
    ResourceManager::Instance()->loadShader("assets/shaders/sprite.vert", "assets/shaders/sprite.frag", "sprite");
    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "batch");
    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/text.frag", "textBatch");

    //NOTE: So far not sure if sprite class will be used, due to sprite batch existance
    testSprite = new Sprite("player", "sprite");
    testSprite->init(-250, 10, playerSpritesheet->getWidth(), playerSpritesheet->getHeight());

    spriteBatch.init();
    textBatch.init();

    spriteFont = new SpriteFont("assets/fonts/chintzy.ttf", 32);
    freeType = new FreeType("assets/fonts/PixelEmulator.ttf", 16);

    // Main game player
    player = new Player();
    //player->init(glm::vec3(100.0f, 100.0f, 0.5f), HUMAN_SPEED, playerSpritesheet, &camera, &bullets, 48.0f, 1.0f);
    player->init(
            glm::vec3(100.0f, 100.0f, 0.5f),
            HUMAN_SPEED,
            ResourceManager::Instance()->loadTexture("assets/textures/player2.png", "player2"),
            &camera,
            &bullets,
            128.0f,
            0.25f);
    humans.push_back(player);

    // Load level and set it's rendering batch and active camera
    LevelParser levelParser;
    //level = levelParser.parseLevel("assets/maps/map1.tmx"); //NOTE: relative tileset path does not work in this case because of launcher pwd(project dir)
    level = levelParser.parseLevel("map1.tmx");
    level->setCamera(&camera);
    level->setBatch(&spriteBatch);

    // Randomise some NPCs
    std::mt19937 rndEngine;
    rndEngine.seed(time(nullptr));
    std::uniform_real_distribution<float> xDist(100.0f, level->getWidth() - 100.0f);
    std::uniform_real_distribution<float> yDist(100.0f, level->getHeight() - 100.0f);

    for (int i = 0; i < 98; i++) {
        humans.push_back(new Human);
        //humans.back()->init(glm::vec3(xDist(rndEngine), yDist(rndEngine), 0.5f), HUMAN_SPEED, playerSpritesheet, 48.0f, 1.0f);
        humans.back()->init(glm::vec3(xDist(rndEngine), yDist(rndEngine), 0.5f), HUMAN_SPEED, ResourceManager::Instance()->loadTexture("assets/textures/human.png", "human"), 128.0f, 0.25f);
    }

    for (int i = 0; i < 2; i++) {
        zombies.push_back(new Zombie);
        //zombies.back()->init(glm::vec3(xDist(rndEngine), yDist(rndEngine), 0.5f), ZOMBIE_SPEED, zombieSpritesheet, 32.0f, 1.0f);
        zombies.back()->init(glm::vec3(xDist(rndEngine), yDist(rndEngine), 0.5f), ZOMBIE_SPEED, ResourceManager::Instance()->loadTexture("assets/textures/zombie.png", "zombie2"), 128.0f, 0.25f);
    }

    //Setup guns
    //Gun(std::string title, int rate, int bps, float spr, float bulletSp, float bulletDmg, int bulletLife);
    // Pistol
    player->addGun(new Gun("9mm", 30, 1, 5.0f, 100.0f, 30, 500, "pistol"));
    // Shotgun
    player->addGun(new Gun("Shotgun", 60, 20, 20.0f, 100.0f, 4, 500, "shotgun"));
    // Machine gun
    player->addGun(new Gun("Uzi", 5, 10, 10.0f, 100.0f, 20, 500, "rifle"));
}

Game::~Game() {
    for (int i = 0; i < zombies.size(); i++) {
        delete zombies[i];
    }
    for (int i = 0; i < humans.size(); i++) {
        delete humans[i];
    }

    SoundManager::Instance()->cleanSoundMaps();
    //ResourceManager::Instance()->clearTextureMap();
}

void Game::run() {
    TheEngine::Instance()->run();
    colorTimer.start();
}

void Game::handleEvents() {
    if(TheInputManager::Instance()->isKeyDown(SDLK_q))
        camera.setZoom(camera.getZoom() + 0.01f);
    if(TheInputManager::Instance()->isKeyDown(SDLK_e))
        camera.setZoom(camera.getZoom() - 0.01f);

    // Get SDL window mouse coords and convert to camera woorld coords
    glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
    mouseCoords = camera.screenToWorld(mouseCoords);

    // Format message and add it in debug console
    std::ostringstream ss;
    ss << "Mouse world position: " << mouseCoords.x << ", " << mouseCoords.y;
    DebugConsole::Instance()->setInfo("mouse", ss.str());

    // On mouse click fire a bullet
    //if (TheInputManager::Instance()->isKeyDown(SDL_BUTTON_LEFT)) {
        ////glm::vec2 playerPos(0.0f); // for now assume player is always in the center
        //glm::vec2 playerPos = glm::vec2(player->getPosition().x, player->getPosition().y);
        //glm::vec2 direction = mouseCoords - playerPos;
        //direction = glm::normalize(direction);

        //bullets.emplace_back(playerPos, direction, 1.0f, 500);
    //}
}

void Game::preUpdate(float dt) {

    // Check victory conditions
    if (zombies.empty()) {
        // WIN
        std::cout << "WIN!\n";
        std::cout << "Zombies Killed:" << numZombiesKilled << "\n";
        std::cout << "Humans Killed:" << numHumansKilled << "\n";
        std::cout << "Humans Remaining:" << humans.size() - 1 << "\n";
        exitWithError("WIN!");
    }

    handleEvents();

    for (int i = 0; i < humans.size();i++) {
        humans[i]->update(dt, *level, humans, zombies);
    }
    for (int i = 0; i < zombies.size();i++) {
        zombies[i]->update(dt, *level, humans, zombies);
    }

    // Update bullets - lifetime, position, level collision
    for (int i = 0; i < bullets.size();) {
        if (bullets[i].update(dt, *level)) {
            //// Remove bullet
            bullets[i] = bullets.back();
            bullets.pop_back();
        } else {
            i++;
        }
    }

    bool bulletRemoved;
    // Bullet - agent collision
    for (int i = 0; i < bullets.size(); i++) {
        bulletRemoved = false;
        for (int j = 0; j < zombies.size();) {
            if (bullets[i].collideWithAgent(zombies[j])) {
                addBlood(glm::vec2(bullets[i].getPosition()), 5);

                if (zombies[j]->applyDamage(bullets[i].getDamage())) {
                    delete zombies[j];
                    zombies[j] = zombies.back();
                    zombies.pop_back();
                    numZombiesKilled++;
                } else {
                    j++;
                }

                bullets[i] = bullets.back();
                bullets.pop_back();
                bulletRemoved = true;
                i--;
                break;
            } else {
                j++;
            }
        }
        if (!bulletRemoved) {
            // Bullet human collision, excluding player so start j from 1
            for (int j = 1; j < humans.size();) {
                if (bullets[i].collideWithAgent(humans[j])) {
                    addBlood(glm::vec2(bullets[i].getPosition()), 5);

                    if (humans[j]->applyDamage(bullets[i].getDamage())) {
                        delete humans[j];
                        humans[j] = humans.back();
                        humans.pop_back();
                        numHumansKilled++;
                    } else {
                        j++;
                    }

                    bullets[i] = bullets.back();
                    bullets.pop_back();
                    i--;
                    break;
                } else {
                    j++;
                }
            }

        }
    }

    //Npc collision and game logic
    for (int i = 0; i < zombies.size(); i++) {
        // Collide zombie/zombie
        for (int j = i + 1; j < zombies.size(); j++) {
            zombies[i]->collideWithAgent(zombies[j]);
        }
        // Collide zombie/human
        // Start from 1, because 0 is player
        for (int j = 1; j < humans.size(); j++) {
            if (zombies[i]->collideWithAgent(humans[j])) {
                // Turn human into zombie - add new zombie and delete human
                zombies.push_back(new Zombie);
                //zombies.back()->init(humans[j]->getPosition(), ZOMBIE_SPEED, zombieSpritesheet, 32.0f, 1.0f);
                zombies.back()->init(humans[j]->getPosition(), ZOMBIE_SPEED, ResourceManager::Instance()->loadTexture("assets/textures/zombie.png", "zombie2"), 128.0f, 0.25f);
                delete humans[j];
                humans[j] = humans.back();
                humans.pop_back();
                SoundManager::Instance()->playSound("zombie");
            }
        }

        //Player collision
        if (zombies[i]->collideWithAgent(player)) {
            std::cout << "You suck!\n";
        }
    }

    // Human/human collision
    for (int i = 0; i < humans.size(); i++) {
        // Only check each pair once!
        for (int j = i + 1; j < humans.size(); j++) {
            humans[i]->collideWithAgent(humans[j]);
        }
    }

    particleEngine.update(dt);

    // Center camera around player
    float cameraZ = camera.getPosition().z;
    glm::vec3 cameraPos = glm::vec3(player->getPosition().x, player->getPosition().y, cameraZ);
    camera.setPosition(cameraPos);
}

void Game::postUpdate(float dt) {
    level->update();

}

void Game::preRender(float dt) {

    // Bind texture
    // Set uniforms
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.2f));
    model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    //glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 projection = glm::ortho(0.0f, (float)TheEngine::Instance()->getScreenWidth(), 0.0f, (float)TheEngine::Instance()->getScreenHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    float colorMin = 0.0f, colorMax = 255.0f, colorCurr = colorMin;
    colorCurr+= (int)colorTimer.read() % 255;
    if (colorCurr > colorMax) {
        colorCurr = colorMin;
    }

    Shader* spriteShader = ResourceManager::Instance()->getShader("sprite");
    if (spriteShader != nullptr) {
        spriteShader->bind();
        spriteShader->setUniformMat4f("model", model);
        spriteShader->setUniformMat4f("view", view);
        spriteShader->setUniformMat4f("projection", projection);
        spriteShader->setUniform1i("spriteTexture", 0);
        //spriteShader->setUniformVec4("uColor", glm::vec4(1.0f, (float)(colorTimer.read() / 255.0f), 0.0f, 1.0f));
        spriteShader->setUniformVec4("uColor", glm::vec4(1.0f, colorCurr, 0.0f, 1.0f));
        //spriteShader->setUniformVec4("uColor", glm::vec4(0.5f, 1.0f, 0.2f, 1.0f));
        testSprite->draw();
    }

    // Setting up rendering batch and rendering it all at once with a single draw call
    Shader* batchShader = ResourceManager::Instance()->getShader("batch");
    if (batchShader != nullptr) {
        batchShader->bind();
        //spriteShader->setUniformMat4f("model", model);
        batchShader->setUniformMat4f("view", view);
        batchShader->setUniformMat4f("projection", projection);
        batchShader->setUniform1i("spriteTexture", 0);
        spriteBatch.begin();

        level->render();

        glm::vec2 agentPosition;
        glm::vec2 agentDimensions;

        for (int i = 0; i < humans.size(); i++) {
            agentPosition = glm::vec2(humans[i]->getPosition());
            agentDimensions = glm::vec2(humans[i]->getSize());
            // Camera Culling! Only draw agents in camera clip space
            if (camera.quadInView(agentPosition, agentDimensions)) {
                humans[i]->draw(spriteBatch);
            }
        }

        for (int i = 0; i < zombies.size(); i++) {
            // TODO: Cull these with Camera2D but first sprite dimensions must be refactored
            zombies[i]->draw(spriteBatch);
        }


        glm::vec4 position(0.0f, 0.0f, 50.0f, 50.0f);
        //glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 uv(0.0f, 0.0f, 1.0f / 6, 1.0f / 10);
        glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        //testBatch->draw(position, uv, playerSpritesheet->getID(), 0.0f, color);
        spriteBatch.draw(position + glm::vec4(100.0f, 0.0f, 0.0f, 0.0f), uv, playerSpritesheet->getID(), 0.5f, color);

        static int currentFrame = 0;
        static int numFrames = 6;
        currentFrame =  int(((SDL_GetTicks() / 100) % 5));
        spriteBatch.draw(position, currentFrame++, 2, 48, 48, playerSpritesheet, 0.5f, color);

        spriteBatch.draw(position + glm::vec4(50.0f, 0.0f, 0.0f, 0.0f), currentFrame, 0, 48, 48, playerSpritesheet, 0.5f, color);


        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].draw(spriteBatch);
        }

        spriteBatch.end();

        spriteBatch.renderBatch();
    }

    particleEngine.draw(&spriteBatch);

    Shader* textShader = ResourceManager::Instance()->getShader("textBatch");
    if (textShader != nullptr) {
        glm::vec4 color(0.0f, 0.2f, 1.0f, 1.0f);
        textShader->bind();
        textShader->setUniformMat4f("view", hudCamera.getCameraMatrix());
        textShader->setUniformMat4f("projection", projection);
        textShader->setUniform1i("spriteTexture", 0);

        textBatch.begin();

        //spriteFont->draw(textBatch, "TESTING", hudCamera.screenToWorld(glm::vec2(10.0f, 150.0f)), glm::vec2(3.0f), 0.6f, color);
        std::stringstream ss;
        ss << "Humans: " << humans.size();
        freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 10.0f)), 2.0f, 0.6f, color);
        ss.str(""); // Empty string stream
        ss << "Zombies: " << zombies.size();
        freeType->draw(textBatch, ss.str(), hudCamera.screenToWorld(glm::vec2(10.0f, 40.0f)), 2.0f, 0.6f, color);

        textBatch.end();

        textBatch.renderBatch();
    }
}


void Game::addBlood(const glm::vec2& pos, int numParticles) {
    static std::mt19937 randEngine(time(nullptr));
    static std::uniform_real_distribution<float> randAngle(0.0f, 2.f * M_PI);
    glm::vec2 vel(2.0f, 0.0f);
    glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < numParticles; i++) {
        bloodParticles->addParticle(pos, glm::rotate(vel, randAngle(randEngine)), color, 20.0f);
    }
}

void Game::postRender(float dt) {
}

void Game::onWindowResize(int newWidth, int newHeight) {
   camera.init(newWidth, newHeight);
}

