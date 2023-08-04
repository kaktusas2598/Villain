#include "StateParser.hpp"

#include "ErrorHandler.hpp"
#include "ResourceManager.hpp"
#include "ScriptEngine.hpp"
#include "IGameScreen.hpp"

#include "Logger.hpp"
#include <sstream>

namespace Villain {
    bool StateParser::loadStates(const char* stateFile, std::vector<IGameScreen*> *gameStates) {
        tinyxml2::XMLDocument xmlDoc;

        if (!xmlDoc.LoadFile(stateFile)) {
            //exitWithError("could not load xml state file");
            std::ostringstream ss;
            ss << "Could not load xml state file: " << stateFile;
            Logger::Instance()->error(ss.str().c_str());
        }

        tinyxml2::XMLElement *root = xmlDoc.RootElement();

        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            //GameState* state = new GameState(e->Value());
            IGameScreen* state = new CustomXMLScreen();
            std::string nextId = e->Attribute("next");
            //state->setNextID(nextId);
            gameStates->push_back(state);
        }


        return true;
    }

    bool StateParser::parseState(const char* stateFile, std::string stateID, std::vector<std::string> *pTextureIDs, std::vector<std::string> *pSoundsIDs) {
        tinyxml2::XMLDocument xmlDoc;

        if (!xmlDoc.LoadFile(stateFile)) {
            exitWithError("Could not load XML state file");
        }

        tinyxml2::XMLElement *root = xmlDoc.RootElement();

        // Find state
        tinyxml2::XMLElement *stateRoot = nullptr;
        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == stateID) {
                stateRoot = e;
            }
        }

        if (!stateRoot) {
            VILLAIN_ERROR("State XML root not found in {}", stateFile);
            return false;
        }

        tinyxml2::XMLElement *textureRoot = nullptr;
        for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("TEXTURES")) {
                textureRoot = e;
            }
        }
        if (textureRoot) parseTextures(textureRoot, pTextureIDs);

        tinyxml2::XMLElement *fontRoot = nullptr;
        for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("FONTS")) {
                fontRoot = e;
            }
        }
        if (fontRoot) parseFonts(fontRoot);

        tinyxml2::XMLElement *soundRoot = 0;
        for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("SOUNDS")) {
                soundRoot = e;
            }
        }

        if (soundRoot) parseSounds(soundRoot, pSoundsIDs);

        tinyxml2::XMLElement *scriptRoot = 0;
        for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("SCRIPTS")) {
                scriptRoot = e;
            }
        }

        if (scriptRoot) parseScripts(scriptRoot);

        // tinyxml2::XMLElement *mapRoot = 0;
        // for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        //     if (e->Value() == std::string("MAPS")) {
        //         mapRoot = e;
        //     }
        // }

        return true;
    }

    void StateParser::parseTextures(tinyxml2::XMLElement* pStateRoot, std::vector<std::string> *pTextureIDs) {
        Logger::Instance()->info("Loading textures.");
        for (tinyxml2::XMLElement *e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            std::string filename = e->Attribute("filename");
            std::string id = e->Attribute("ID");
            pTextureIDs->push_back(id);
            ResourceManager::Instance()->loadTexture(filename, id);
        }
    }

    // TODO: for not state will only parse fonts and won't clean them up, I know this is bad, don't judge me
    void StateParser::parseFonts(tinyxml2::XMLElement* pStateRoot) {
        Logger::Instance()->info("Loading fonts.");
        for (tinyxml2::XMLElement *e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            std::string filename = e->Attribute("filename");
            std::string id = e->Attribute("ID");
            // TODO:
            //TheTextureManager::Instance()->loadFont(filename, id);
        }

	}

    void StateParser::parseSounds(tinyxml2::XMLElement* pStateRoot, std::vector<std::string> *pSoundIDs) {
        Logger::Instance()->info("Loading sounds.");
        //for (tinyxml2::XMLElement *e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            //std::string filename = e->Attribute("filename");
            //std::string id = e->Attribute("ID");
            //int type  = e->IntAttribute("type");
            ////e->Attribute("type", &type);
            //SoundType soundType;
            //switch (type) {
            //case 0:
                //soundType = SOUND_MUSIC;
                //break;
            //default:
                //soundType = SOUND_SFX;
                //break;
            //}
            //pSoundIDs->push_back(id);
            //SoundManager::Instance()->load(filename, id, soundType);
        //}
    }

    void StateParser::parseScripts(tinyxml2::XMLElement* pScriptRoot) {
        Logger::Instance()->info("Initialising scripts.");
        for (tinyxml2::XMLElement *e = pScriptRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            std::string name, fileName, type;

            name = e->Attribute("name");
            fileName = e->Attribute("filename");
            type = e->Attribute("type");

            ScriptEngine::Instance()->init(fileName);
            if (type == "entity") {
            }
        }
    }

    // This is the most basic approach so far, only loads first found map
    // But maybe 1 map/level is all we want?
    std::string StateParser::getMapName(const char* stateFile, std::string stateID) {
        tinyxml2::XMLDocument xmlDoc;

        if (!xmlDoc.LoadFile(stateFile)) {
            exitWithError("Could not load XML state file");
        }

        tinyxml2::XMLElement *root = xmlDoc.RootElement();

        // Find state
        tinyxml2::XMLElement *stateRoot = 0;
        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == stateID) {
                stateRoot = e;
            }
        }

        if (stateRoot != 0) {
            tinyxml2::XMLElement *mapRoot = 0;
            for (tinyxml2::XMLElement *e = stateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
                if (e->Value() == std::string("MAPS")) {
                    mapRoot = e;
                }
            }

             if (mapRoot != 0) {
                for (tinyxml2::XMLElement *e = mapRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
                    std::string fileName;

                    fileName = e->Attribute("filename");
                    return fileName;
                }
             }
        }

        return std::string();
     }
}
