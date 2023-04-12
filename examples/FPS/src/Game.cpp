#include "Game.hpp"

#include "Light.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"

#include "Level.hpp"

using namespace Villain;


void Game::init() {
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVA1.png", "walk1");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVB1.png", "walk2");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVC1.png", "walk3");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVD1.png", "walk4");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVE1.png", "walk5");

    ResourceManager::Instance()->loadTexture("assets/textures/SSWVE0.png", "fire1");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVF0.png", "fire2");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVG0.png", "fire3");

    ResourceManager::Instance()->loadTexture("assets/textures/SSWVH0.png", "pain");

    ResourceManager::Instance()->loadTexture("assets/textures/SSWVI0.png", "die1");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVJ0.png", "die2");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVK0.png", "die3");
    ResourceManager::Instance()->loadTexture("assets/textures/SSWVL0.png", "die4");

    ResourceManager::Instance()->loadTexture("assets/textures/SSWVM0.png", "death");

    ResourceManager::Instance()->loadTexture("assets/textures/PISGB0.png", "gun");

    // Light test
    // NOTE: Noticed if I add lights here, it messes up blending on the sprite a bit - it still is transparent but has greenish shade
    // Probably related to RenderingEngine using additive blending to mix up light color with existing one in default framebuffer?
    //glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.f);
    //SceneNode* pointLight = ((new SceneNode("Point Light 1", glm::vec3(4.f, 2.f, 3.f)))
                //->addComponent(new PointLight(redLight * glm::vec3(0.2f), redLight, glm::vec3(1.0f),glm::vec3(100.0f, 2.0f, -10.0f), 1.0f, 0.022f, 0.0019f)));
    //addToScene(pointLight);

    //glm::vec3 lightColor = glm::vec3(0.5f, 0.7f, 0.4f);
    //SceneNode* spotLight = ((new SceneNode("Spot Light"))
                //->addComponent(new SpotLight(lightColor * glm::vec3(0.2f), lightColor, glm::vec3(1.0f), glm::vec3(20.f, 20.f, 10.f), glm::vec3(0.0f, -5.f, 0.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))[>, &camera<])));
    //addToScene(spotLight);

    hudCamera.rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
    glm::vec3 camPos = hudCamera.getPosition();
    camPos.y = Engine::getScreenHeight()/2.0;
    hudCamera.setPosition(camPos);

    spriteBatch.init();
    hudShader = new Shader();
    hudShader->createFromResource("spriteBatch");

    // Add level which will also generate and add to scene all special objects
    currentLevel = new Level("assets/bitmaps/level1.png", "assets/textures/WolfCollection.png", this);
}

void Game::onAppPreUpdate(float dt) {
    if (InputManager::Instance()->isKeyDown(SDLK_ESCAPE)) {
        Engine::setRunning(false);
    }
}

void Game::onAppRender(float dt) {
    // HUD DRAW/ GUN
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f));

    if (hudShader != nullptr) {
        hudShader->bind();
        hudShader->setUniformMat4f("model", model);
        hudShader->setUniformMat4f("view", hudCamera.getViewMatrix());
        hudShader->setUniformMat4f("projection", hudCamera.getProjMatrix());
        hudShader->setUniform1i("spriteTexture", 0);
        spriteBatch.begin();

        glm::vec4 uvRect;
        uvRect.x = 0.0f;
        uvRect.y = 0.0f;
        uvRect.z = 1.0f;
        uvRect.w = 1.0f;
        glm::vec4 destRect{-50.0f, 0.0f, 100.f, 100.f};
        spriteBatch.draw(destRect, uvRect, ResourceManager::Instance()->getTexture("gun")->getID(), -1.0f, glm::vec4(1.0f));

        spriteBatch.end();
        spriteBatch.renderBatch();
    }

}
