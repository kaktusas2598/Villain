#pragma once

#include "SceneNode.hpp"
#include "tinyxml2/tinyxml2.h"

namespace Villain {

    // Parses scene graph from XML file, load and save scene
    class SceneParser {
        public:
            void loadSceneGraph(const std::string& fileName, SceneNode* rootNode);
            void saveSceneGraph(const std::string& fileName, SceneNode* rootNode);
        private:
            void parseSceneNode(tinyxml2::XMLElement* nodeRoot, SceneNode* parentNode);
            void parseProperties(tinyxml2::XMLElement* e, SceneNode* rootNode);

    };
}
