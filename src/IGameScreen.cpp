#include "IGameScreen.hpp"

#include "Engine.hpp"
#include "StateParser.hpp"
//#include "LevelParser.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"
//#include "EntityManager.hpp"
#include "ScriptEngine.hpp"
#include "Logger.hpp"

namespace Villain {

    CustomXMLScreen::CustomXMLScreen() {}
    CustomXMLScreen::~CustomXMLScreen() {}

    // TODO:
    int CustomXMLScreen::getNextScreenID() const {
        return STATE_NONE;
    }

    // TODO:
    int CustomXMLScreen::getPrevScreenID() const {
        return STATE_NONE;
    }

    // TODO:
    void CustomXMLScreen::build() {
    }

    // TODO:
    void CustomXMLScreen::destroy() {
    }

    //TODO:
    void CustomXMLScreen::onEntry() {
        // Allocate on stack one time utilities for state loading
        StateParser stateParser;
        //LevelParser levelParser;
        std::string map;
        //stateParser.parseState("state.xml", stateID, &textureIDs, &soundIDs);
        //map = stateParser.getMapName("state.xml", stateID);

        //if (!map.empty()) {
        //Logger::Instance()->info("Loading level.");
        //level = levelParser.parseLevel(map.c_str());
        ////TheEngine::Instance()->setLevel(level);
        //}
    }

    //TODO:
    void CustomXMLScreen::onExit() {
        //for(size_t i = 0; i < textureIDs.size(); i++) {
        //ResourceManager::Instance()->clearTexture(textureIDs[i]);
        //}

        SoundManager::Instance()->cleanSoundMaps();

        ScriptEngine::Instance()->close();

        //EntityManager::Instance()->clean();

        //if (level != nullptr) {
        //for (auto id : level->getTextureIDs())
        //ResourceManager::Instance()->clearTexture(id);
        //delete level;
        ////TheEngine::Instance()->setLevel(nullptr);
        //}
    }

    //TODO:
    void CustomXMLScreen::update(float deltaTime) {
        //if (level != nullptr)
        //level->update();

        if (Input::Get()->isKeyPressed(SDLK_ESCAPE)) {
            mainApplication->getStateMachine()->getCurrentScreen()->setScreenState(ScreenState::CHANGE_NEXT);
        }

        // Make sure camera doesn't go out of bounds of map
        //if (TheEngine::Instance()->camera.x < 0) {
        //TheEngine::Instance()->camera.x = 0;
        //}
        //if (TheEngine::Instance()->camera.y < 0) {
        //TheEngine::Instance()->camera.y = 0;
        //}

        //int xMax = (level == nullptr) ? TheEngine::Instance()->camera.w : (level->getWidth() - TheEngine::Instance()->camera.w);
        //int yMax = (level == nullptr) ? TheEngine::Instance()->camera.h : (level->getHeight() - TheEngine::Instance()->camera.h);

        //if (TheEngine::Instance()->camera.x > xMax) {
        //TheEngine::Instance()->camera.x = xMax;
        //}
        //if (TheEngine::Instance()->camera.y > yMax) {
        //TheEngine::Instance()->camera.y = yMax;
        //}
    }

    //TODO:
    void CustomXMLScreen::draw() {
    }

}
