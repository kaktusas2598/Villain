#ifndef __SCENE_NODE__
#define __SCENE_NODE__

#include "Transform.hpp"
#include <vector>

namespace Villain {

    class Camera;
    class Engine;
    class NodeComponent;
    class RenderingEngine;
    class Shader;

    // Represents a single node in the Scene Graph structure
    class SceneNode {
        public:
            SceneNode(const glm::vec3& pos = glm::vec3(0.f), const glm::quat& rot = glm::quat(0, 0, 0, 0), float scale = 1.0f);
            ~SceneNode();

            void handleInput();
            void update(float deltaTime);
            void render(Shader* shader, RenderingEngine* renderingEngine, Camera* camera);

            SceneNode* addChild(SceneNode* child);
            SceneNode* addComponent(NodeComponent* component);

            //std::vector<SceneNode*>& getAllAttached();

            Transform* getTransform() { return &transform; }
            void setEngine(Engine* e);
            inline Engine* getEngine() { return engine; }
        private:
            Transform transform;
            std::vector<SceneNode*> children;
            std::vector<NodeComponent*> components;
            Engine* engine = nullptr;

            SceneNode(const SceneNode& other) {}
            void operator=(const SceneNode& other) {}
    };
};

#endif // __SCENE_NODE__
