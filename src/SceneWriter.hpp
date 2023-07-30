#pragma once

#include "SceneNode.hpp"
#include "tinyxml2/tinyxml2.h"

namespace Villain {

    /// Saves scene graph from application to xml file for later loading
    class SceneWriter {
        public:
            void saveSceneGraph(const std::string& fileName, SceneNode* rootNode);
        private:
            void saveSceneNode(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parentElement, SceneNode* currentNode);
    };
}
