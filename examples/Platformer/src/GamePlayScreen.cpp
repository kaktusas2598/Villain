#include "GamePlayScreen.hpp"
#include "glm/fwd.hpp"

#include <random>

GamePlayScreen::GamePlayScreen() {}

GamePlayScreen::~GamePlayScreen() {}

int GamePlayScreen::getNextScreenID() const {
    return STATE_NONE;
}


int GamePlayScreen::getPrevScreenID() const {
    return STATE_NONE;
}

void GamePlayScreen::build() {

}

void GamePlayScreen::destroy() {

}

void GamePlayScreen::onEntry() {
    boxTexture = Villain::ResourceManager::Instance()->loadTexture("assets/textures/crate.png", "box");
    playerSpriteAtlas = Villain::ResourceManager::Instance()->loadTexture("assets/textures/player.png", "player");

    b2Vec2 gravity(0.0f, -9.81f);
    //b2Vec2 gravity(0.0f, -14.81f);
    world = std::make_unique<b2World>(gravity);

    // Make ground
    b2BodyDef groundBodyDef;
    // Everything in box2d are meters!
    groundBodyDef.position.Set(0.0f, -20.0f);
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
    //ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/text.frag", "textBatch");

    spriteBatch.init();
    //textBatch.init();

    //spriteFont = new SpriteFont("assets/fonts/chintzy.ttf", 32);
    //freeType = new FreeType("assets/fonts/PixelEmulator.ttf", 16);

    camera.init(mainApplication->getScreenWidth(), mainApplication->getScreenHeight());
    // Zoom out because Box2D uses meters and not pixels
    camera.setZoom(32.0f);
    //hudCamera.init(configScript.get<int>("window.width"), configScript.get<int>("window.height"));
    //glm::vec3 camPos = camera.getPosition();
    //camPos.x = mainApplication->getScreenWidth()/2.0;
    //camPos.y = mainApplication->getScreenHeight()/2.0;
    //camera.setPosition(camPos);

    player.init(world.get(), glm::vec2(0.0f, 30.0f), glm::vec2(1.2f, 1.8f), playerSpriteAtlas->getID(), glm::vec4(1.0f));
}

void GamePlayScreen::onExit() {

}

void GamePlayScreen::update(float deltaTime) {
    player.update();
    // Update physics
    world->Step(deltaTime, 6, 2);
}

void GamePlayScreen::draw(float deltaTime) {

    // Setting up rendering batch and rendering it all at once with a single draw call
    if (textureShader != nullptr) {
        textureShader->bind();
        // Set uniforms
        textureShader->setUniformMat4f("view", camera.getCameraMatrix());
        textureShader->setUniformMat4f("projection", glm::mat4(1.0f));
        textureShader->setUniform1i("spriteTexture", 0);

        spriteBatch.begin();

        for(auto& b : boxes) {
            b.draw(spriteBatch);
        }

        player.draw(spriteBatch);

        spriteBatch.end();

        spriteBatch.renderBatch();
        textureShader->bind();
    }
}

