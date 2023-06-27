#ifndef __SCENE_NODE__
#define __SCENE_NODE__

#include "Transform.hpp"
#include <bitset>
#include <string>
#include <typeindex>
#include <vector>

namespace Villain {


    typedef unsigned long long EntityID;
    static EntityID EntityCount = 0;

    class Camera;
    class Engine;
    class NodeComponent;
    class RenderingEngine;
    class Shader;

    // Represents a single node in the Scene Graph structure
    class SceneNode {
        public:
            SceneNode(const std::string& name, const glm::vec3& pos = glm::vec3(0.f), const glm::vec3& rot = glm::vec3(0.f), float scale = 1.0f);
            ~SceneNode();

            void handleInput(float deltaTime);
            void update(float deltaTime);
            void render(Shader* shader, RenderingEngine* renderingEngine, Camera* camera);

            SceneNode* addChild(SceneNode* child);
            SceneNode* addComponent(NodeComponent* component);

            void removeChild(SceneNode* child);
            //std::vector<SceneNode*>& getAllAttached();

            bool isSelected() const { return selected; }
            void setSelected(bool select) { selected = select; }

            EntityID getID() const { return id; }

            SceneNode* findByID(EntityID iD) {
                if (id == iD)
                    return this;
                for (unsigned int i = 0; i < children.size(); i++) {
                    SceneNode* child = children[i]->findByID(iD);
                    if (child != nullptr) return child;
                }
                return nullptr;
            }
            template <typename T> T* getComponent();

            Transform* getTransform() { return &transform; }
            void setEngine(Engine* e);
            inline Engine* getEngine() { return engine; }
            const std::string& getName() const { return uid; }
            void setName(const std::string& name) { uid = name; }

            std::vector<SceneNode*>& getChildren() { return children; }
            std::vector<NodeComponent*>& getComponents() { return components; }
        private:
            EntityID id;
            std::string uid;

            Transform transform;
            std::vector<SceneNode*> children;
            std::vector<NodeComponent*> components;
            Engine* engine = nullptr;

            bool selected = false;

            SceneNode(const SceneNode& other) {}
            void operator=(const SceneNode& other) {}
    };
};

#endif // __SCENE_NODE__
