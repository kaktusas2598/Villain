#include <iostream>
#include <vector>
#include "tinyxml2/tinyxml2.h"

namespace Villain {

    class Entity;
    class IGameScreen;

    // TODO: port this completely to Villain, allow optional usage of XML states
    /***
     * Parses XML state file using tinyxml, for example of required structure look at state.xml file
     */
    class StateParser {
        public:
            // Parse single state, load assets and script
            bool parseState(const char* stateFile, std::string stateID, std::vector<std::string> *pTextureIDs, std::vector<std::string> *pSoundsIDs);
            bool loadStates(const char* stateFile, std::vector<IGameScreen*> *gameStates);
            std::string getMapName(const char* stateFile, std::string stateID);
        private:
            void parseObjects(tinyxml2::XMLElement* pStateRoot, std::vector<Entity*> *entities);
            void parseTextures(tinyxml2::XMLElement* pStateRoot, std::vector<std::string> *pTextureIDs);
            void parseFonts(tinyxml2::XMLElement* pStateRoot);
            void parseSounds(tinyxml2::XMLElement* pStateRoot, std::vector<std::string> *pSoundIDs);
            void parseScripts(tinyxml2::XMLElement* pScriptRoot);
            // TODO: parse Models, Shaders, scenes
    };
}
