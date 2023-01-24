#include <iostream>
#include <string>

#include "Engine.hpp"
#include "LuaScript.hpp"
#include "Sprite.hpp"
#include "Camera2D.hpp"
#include "ResourceManager.hpp"

using namespace Villain;

Sprite* testSprite = nullptr;
Camera2D camera;

void preUpdate(float deltaTime) {
    if(TheInputManager::Instance()->isKeyDown(SDLK_w))
        camera.setPosition(camera.getPosition() + glm::vec3(0.0f, 10.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_a))
        camera.setPosition(camera.getPosition() + glm::vec3(-10.0f, 0.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_s))
        camera.setPosition(camera.getPosition() + glm::vec3(0.0f, -10.0f, 0.0f));
    if(TheInputManager::Instance()->isKeyDown(SDLK_d))
        camera.setPosition(camera.getPosition() + glm::vec3(10.0f, 0.0f, 0.0f));
}

void postUpdate(float deltaTime) {
}

void preRender(float deltaTime) {
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
}

void postRender(float deltaTime) {
}


int main(int argc, char *argv[]) {

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
    TheEngine::Instance()->init(
            configScript.get<std::string>("window.title"),
            configScript.get<int>("window.height"),
            configScript.get<int>("window.width"),
            flags
            );

    camera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));

    Texture* playerSpritesheet = ResourceManager::Instance()->loadTexture("assets/textures/player.png", "player");
    ResourceManager::Instance()->loadShader("assets/shaders/sprite.vert", "assets/shaders/sprite.frag", "sprite");

    testSprite = new Sprite("player", "sprite");
    testSprite->init(10, 10, playerSpritesheet->getWidth(), playerSpritesheet->getHeight());


    TheEngine::Instance()->run();

    return 0;
}
