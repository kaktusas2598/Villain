#pragma once

#include "SceneNode.hpp"
#include "tinyxml2/tinyxml2.h"

namespace Villain {

    /// Saves scene graph from application to xml file for later loading
    // TODO: Implement this
    class SceneWriter {
        public:
            void saveSceneGraph(const std::string& fileName, SceneNode* rootNode) {}
        private:

    };
}
