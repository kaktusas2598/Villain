#include "GamePlayScreen.hpp"
#include "Input.hpp"
#include "glm/fwd.hpp"

#include "Light2D.hpp"
#include <random>

GamePlayScreen::GamePlayScreen() {}

GamePlayScreen::~GamePlayScreen() {}

int GamePlayScreen::getNextScreenID() const {
    return 0;
}


int GamePlayScreen::getPrevScreenID() const {
    return 0;
}

void GamePlayScreen::build() {

}

void GamePlayScreen::destroy() {

}

void GamePlayScreen::onEntry() {
    SDL_ShowCursor(0);

    debugRenderer.init();

    boxTexture = Villain::ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "box");
    playerSpriteAtlas = Villain::ResourceManager::Instance()->loadTexture("assets/textures/player.png", "player");

    b2Vec2 gravity(0.0f, -9.81f);
    //b2Vec2 gravity(0.0f, -14.81f);
    world = std::make_unique<b2World>(gravity);

    // Make ground
    b2BodyDef groundBodyDef;
    // Everything in box2d are meters!
    groundBodyDef.position.Set(0.0f, -25.0f);
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    // Make the ground fixture
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 8.0f); // size in meters
    groundBody->CreateFixture(&groundBox, 0.0f);

    std::mt19937 randGenerator((unsigned int)time(nullptr));
    std::uniform_real_distribution<float> xDist(-10.0f, 10.0f);
    std::uniform_real_distribution<float> yDist(-15.0f, 15.0f);
    std::uniform_real_distribution<float> size(0.5f, 2.0f);
    std::uniform_int_distribution<int> color(0, 255);
    const int NUM_BOXES = 50;

    for (int i = 0; i < NUM_BOXES; i++) {
        glm::vec4 randColor;
        randColor.r = color(randGenerator) / 255.f;
        randColor.g = color(randGenerator) / 255.f;
        randColor.b = color(randGenerator) / 255.f;
        randColor.a = 1.0f;
        Box newBox;
        newBox.init(world.get(), glm::vec2(xDist(randGenerator), yDist(randGenerator)), glm::vec2(size(randGenerator)), boxTexture->getID(), randColor);
        boxes.push_back(newBox);
    }

    textureShader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "batch");
    light2DShader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/light2D.vert", "assets/shaders/light2D.frag", "light2D");
    //ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/text.frag", "textBatch");

    spriteBatch.init();
    //textBatch.init();

    //spriteFont = new SpriteFont("assets/fonts/chintzy.ttf", 32);
    //freeType = new FreeType("assets/fonts/PixelEmulator.ttf", 16);

    camera = new Villain::Camera(Villain::CameraType::ORTHOGRAPHIC_2D);
    camera->rescale(Villain::Engine::getScreenWidth(), Villain::Engine::getScreenHeight());
    // Zoom out because Box2D uses meters and not pixels
    camera->setZoom(32.0f);
    //hudCamera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    //glm::vec3 camPos = camera.getPosition();
    //camPos.x = mainApplication->getScreenWidth()/2.0;
    //camPos.y = mainApplication->getScreenHeight()/2.0;
    //camera.setPosition(camPos);

    player.init(world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(2.0f), glm::vec2(0.3f, 0.8f), playerSpriteAtlas->getID(), glm::vec4(1.0f));
}

void GamePlayScreen::onExit() {
    Villain::ResourceManager* rM = Villain::ResourceManager::Instance();
    rM->clearTexture("box");
    rM->clearTexture("player");
    rM->clearTextureMap();

    rM->clearShader("batch");
    rM->clearShader("light2D");
    rM->clearShaderMap();

    boxes.clear();
    world.reset();

    SDL_ShowCursor(1);
}

void GamePlayScreen::update(float deltaTime) {
    player.update();
    // Update physics
    world->Step(deltaTime, 6, 2);

    if (Villain::Input::Get()->isKeyPressed(SDLK_1)) {
        debugRenderMode = !debugRenderMode;
    }
    if (Villain::Input::Get()->isKeyPressed(SDLK_ESCAPE)) {
        setScreenState(Villain::ScreenState::CHANGE_PREVIOUS);
    }
}

void GamePlayScreen::draw() {
    // Setting up rendering batch and rendering it all at once with a single draw call
    if (textureShader != nullptr) {
        textureShader->bind();
        // Set uniforms
        textureShader->setUniformMat4f("view", camera->getViewMatrix());
        textureShader->setUniformMat4f("projection", camera->getProjMatrix());
        textureShader->setUniform1i("spriteTexture", 0);

        spriteBatch.begin();

        for(auto& b : boxes) {
            b.draw(spriteBatch);
        }

        player.draw(spriteBatch);

        spriteBatch.end();

        spriteBatch.renderBatch();
        //textureShader->bind();

        if (debugRenderMode) {
            glm::vec4 destRect;
            for(auto& b : boxes) {
                destRect.x = b.getBody()->GetPosition().x - b.getSize().x / 2.0f;
                destRect.y = b.getBody()->GetPosition().y - b.getSize().y / 2.0f;
                destRect.z = b.getSize().x;
                destRect.w = b.getSize().y;
                debugRenderer.drawBox(destRect, 0.0f, glm::vec4(-0.5f), b.getBody()->GetAngle());
            }
            // Render player's collision box
            player.drawDebug(debugRenderer);
            //auto& b = player.getCapsule();
            //destRect.x = b.getBody()->GetPosition().x - b.getSize().x / 2.0f;
            //destRect.y = b.getBody()->GetPosition().y - b.getSize().y / 2.0f;
            //destRect.z = b.getSize().x;
            //destRect.w = b.getSize().y;
            //debugRenderer.drawBox(destRect, 0.0f, glm::vec4(1.0f), b.getBody()->GetAngle());

            debugRenderer.end();
            debugRenderer.render(camera->getProjMatrix() * camera->getViewMatrix(), 2.0f);
        }

        // HACK: Render test lights
        // TODO: stop hardcoding this
        Light2D playerLight;
        playerLight.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
        playerLight.size = 20.0f;
        glm::vec3 playerPosition;
        playerPosition.x = player.getCapsule().getBody()->GetPosition().x;
        playerPosition.y = player.getCapsule().getBody()->GetPosition().y;
        playerPosition.z = -0.1f;
        playerLight.position = playerPosition;

        Light2D mouseLight;
        mouseLight.color = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
        mouseLight.size = 10.0f;
        glm::vec2 mouseCoords = camera->screenToWorld(Villain::Input::Get()->getMouseCoords());
        //mouseCoords.x += 5.0f / 2;
        //mouseCoords.y += 5.0f / 2;
        mouseLight.position = glm::vec3(mouseCoords.x, mouseCoords.y, -0.4f);

        //// Additive blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        spriteBatch.begin();

        light2DShader->bind();
        light2DShader->setUniformMat4f("MVP", camera->getProjMatrix() * camera->getViewMatrix());

        playerLight.draw(spriteBatch);
        mouseLight.draw(spriteBatch);

        spriteBatch.end();
        spriteBatch.renderBatch();

        // Back to regular blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void GamePlayScreen::onAppWindowResize(int newWidth, int newHeight) {
    std::cout << newWidth << ", " << newHeight << "\n";
    camera->rescale(newWidth, newHeight);
    //glm::vec3 camPos = camera.getPosition();
    //camPos.x = newWidth/2.0;
    //camPos.y = newHeight/2.0;
    //camera.setPosition(camPos);
}
