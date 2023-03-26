#ifndef __SCENE_NODE__
#define __SCENE_NODE__

#include <vector>

namespace Villain {

    class Engine;
    class NodeComponent;

    //TODO: finish implementing

    // Represents a single node in the Scene Graph structure
    class SceneNode {
        public:
            SceneNode();
            ~SceneNode();

            void handleInput();
            void update(float deltaTime);
            void render();

            void addChild(SceneNode* child) { children.push_back(child); }
            void addComponent(NodeComponent* component) { components.push_back(component); }

            void setEngine(Engine* e) { engine = e; }
        private:
            std::vector<SceneNode*> children;
            std::vector<NodeComponent*> components;
            Engine* engine;
    };
};

#endif // __SCENE_NODE__
