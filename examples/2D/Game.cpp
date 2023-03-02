#include "Game.hpp"

#include "Engine.hpp"
#include "LuaScript.hpp"
#include "ResourceManager.hpp"

using namespace Villain;

Sprite* Game::testSprite = nullptr;
SpriteBatch Game::spriteBatch;
Camera2D Game::camera;
Texture* Game::playerSpritesheet = nullptr;
std::vector<Bullet> Game::bullets;


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

    camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));

    playerSpritesheet = ResourceManager::Instance()->loadTexture("assets/textures/player.png", "player");
    ResourceManager::Instance()->loadShader("assets/shaders/sprite.vert", "assets/shaders/sprite.frag", "sprite");
    ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "batch");

    testSprite = new Sprite("player", "sprite");
    testSprite->init(10, 10, playerSpritesheet->getWidth(), playerSpritesheet->getHeight());

    spriteBatch.init();
}

Game::~Game() {}

void Game::run() {
    TheEngine::Instance()->run();
}

void Game::handleEvents() {
    if(TheInputManager::Instance()->isKeyDown(SDLK_w))
        camera.setPosition(camera.getPosition() + glm::vec3(0.0f, 10.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_a))
        camera.setPosition(camera.getPosition() + glm::vec3(-10.0f, 0.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_s))
        camera.setPosition(camera.getPosition() + glm::vec3(0.0f, -10.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_d))
        camera.setPosition(camera.getPosition() + glm::vec3(10.0f, 0.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_q))
        camera.setZoom(camera.getZoom() + 0.01f);
    if(TheInputManager::Instance()->isKeyDown(SDLK_e))
        camera.setZoom(camera.getZoom() - 0.01f);

    if (TheInputManager::Instance()->isKeyDown(SDL_BUTTON_LEFT)) {
        glm::vec2 mouseCoords = TheInputManager::Instance()->getMouseCoords();
        mouseCoords = camera.screenToWorld(mouseCoords);
        std::cout << "Mouse clicked: " << mouseCoords.x << ", " << mouseCoords.y << std::endl;

        glm::vec2 playerPos(0.0f); // for now assume player is always in the center
        glm::vec2 direction = mouseCoords - playerPos;
        direction = glm::normalize(direction);

        bullets.emplace_back(playerPos, direction, 1.0f, 100);
    }

}

void Game::preUpdate(float dt) {
    handleEvents();
}

void Game::postUpdate(float dt) {
    for (int i = 0; i < bullets.size();) {
        if (bullets[i].update()) {
            // Remove bullet
            bullets[i] = bullets.back();
            bullets.pop_back();
        } else {
            i++;
        }
    }
}

void Game::preRender(float dt) {
    // Bind texture
    // Set uniforms
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), float(SDL_GetTicks())* 0.001f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(2.0f));
    //glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 projection = glm::ortho(0.0f, (float)TheEngine::Instance()->getScreenWidth(), 0.0f, (float)TheEngine::Instance()->getScreenHeight(), 0.1f, 100.0f);
    glm::mat4 view = camera.getCameraMatrix();
    glm::mat4 projection = glm::mat4(1.0f);

    Shader* spriteShader = ResourceManager::Instance()->getShader("sprite");
    if (spriteShader != nullptr) {
        spriteShader->bind();
        spriteShader->setUniformMat4f("model", model);
        spriteShader->setUniformMat4f("view", view);
        spriteShader->setUniformMat4f("projection", projection);
        spriteShader->setUniform1i("spriteTexture", 0);
        testSprite->draw();
    }

    Shader* batchShader = ResourceManager::Instance()->getShader("batch");
    if (batchShader != nullptr) {
        batchShader->bind();
        //spriteShader->setUniformMat4f("model", model);
        batchShader->setUniformMat4f("view", view);
        batchShader->setUniformMat4f("projection", projection);
        batchShader->setUniform1i("spriteTexture", 0);
        spriteBatch.begin();

        glm::vec4 position(0.0f, 0.0f, 50.0f, 50.0f);
        //glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 uv(0.0f, 0.0f, 1.0f / 6, 1.0f / 10);
        glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        //testBatch->draw(position, uv, playerSpritesheet->getID(), 0.0f, color);
        spriteBatch.draw(position + glm::vec4(100.0f, 0.0f, 0.0f, 0.0f), uv, playerSpritesheet->getID(), 0.0f, color);

        static int currentFrame = 0;
        static int numFrames = 6;
        currentFrame =  int(((SDL_GetTicks() / 100) % 5));
        spriteBatch.draw(position, currentFrame++, 2, 48, 48, playerSpritesheet, 0.0f, color);

        spriteBatch.draw(position + glm::vec4(50.0f, 0.0f, 0.0f, 0.0f), currentFrame, 0, 48, 48, playerSpritesheet, 0.0f, color);


        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].draw(spriteBatch);
        }

        spriteBatch.end();

        spriteBatch.renderBatch();

    }

}

void Game::postRender(float dt) {
}

void Game::onWindowResize(int newWidth, int newHeight) {
   camera.init(newWidth, newHeight);
}

