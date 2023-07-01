#ifndef __SCENE_GRAPH_EDITOR__
#define __SCENE_GRAPH_EDITOR__

#include "SceneNode.hpp"

namespace Villain {

    class SceneGraphEditor {
        public:
            void render(Engine& engine);

            void setSelectedNode(SceneNode* node) { selectedNode = node; }

        private:
            void drawNodeHierarchy(SceneNode* node); //<<< Render Scene Graph nodes and components recursively
            void drawSelectedNode();

            void drawNodeProperties(SceneNode* node);
            void drawNodeComponents(SceneNode* node);

            SceneNode* selectedNode = nullptr;
    };
}

#endif // __SCENE_GRAPH_EDITOR__
