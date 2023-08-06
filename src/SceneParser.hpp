#pragma once

#include "SceneNode.hpp"
#include "tinyxml2/tinyxml2.h"

namespace Villain {

    /// Parses and reads scene graph from XML file for loading scenes
    class SceneParser {
        public:
            void loadSceneGraph(const std::string& fileName, SceneNode* rootNode);
        private:
            void parseSceneNode(tinyxml2::XMLElement* nodeRoot, SceneNode* parentNode);
            void parseProperties(tinyxml2::XMLElement* e, SceneNode* rootNode);

            /// Helper methods
            std::string GetUniqueId();
            glm::vec3 hexToVec3(const std::string& hexColor);
            glm::vec3 parseColorAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attributeName);
            glm::vec3 parseVec3Element(tinyxml2::XMLElement* element, const std::string& elementName,
                    const std::string& x = "x", const std::string& y = "y", const std::string& z = "z");
            float parseFloatAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attribName);
            bool parseBoolAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attribName);
    };
}
