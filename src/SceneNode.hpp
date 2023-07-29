#pragma once

#include "Transform.hpp"
#include <bitset>
#include <string>
#include <vector>

namespace Villain {

    // Ideas borrowed from SceneGraph, each Node/Entity will have a bitmask to represent which components it has,
    // which will also make it easier to use it in imgui
    typedef unsigned long long EntityID;
    const int MAX_COMPONENTS = 32;
    typedef std::bitset<MAX_COMPONENTS> ComponentMask;

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
            // Must be called from the root node only to handle render order properly for transparency!
            void render(Shader* shader, RenderingEngine* renderingEngine, Camera* camera);

            SceneNode* addChild(SceneNode* child);
            SceneNode* addComponent(NodeComponent* component);

            void removeChild(SceneNode* child);
            void removeComponent(NodeComponent* component);

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
            Transform* getTransform() { return &transform; }
            void setEngine(Engine* e);
            inline Engine* getEngine() { return engine; }
            const std::string& getName() const { return uid; }
            void setName(const std::string& name) { uid = name; }

            // Returns all nodes attached to this traversing through hierachy, useful for rendering and sorting
            std::vector<SceneNode*> getAllAttached();
            std::vector<SceneNode*>& getChildren() { return children; }
            std::vector<NodeComponent*>& getComponents() { return components; }
        private:
            // Calculate distance from camera to node for every attached node
            void collectNodeDistances(Camera* camera, std::vector<std::pair<SceneNode*, float>>& nodeDistances);

            // ECS TODO: component mask can be used if we template addComponent method, but when we
            // want to change it completely, it can't take NodeComponent* anymore
            EntityID id;
            ComponentMask componentMask;

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
