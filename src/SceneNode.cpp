#include "SceneNode.hpp"

#include "NodeComponent.hpp"

#include <algorithm> // For std::remove

namespace Villain {

    SceneNode::SceneNode(const std::string& name, const glm::vec3& pos, const glm::vec3& rot, float scale)
        : uid(name), transform(pos, rot, scale), engine(nullptr) {
            id = EntityCount;
            EntityCount += 1;
    }

    SceneNode::~SceneNode() {
        for (unsigned int i = 0; i < components.size(); i++) {
            if (components[i]) {
                delete components[i];
                components[i] = nullptr;
            }
        }

        for(unsigned int i = 0; i < children.size(); i++) {
            if (children[i]) {
                delete children[i];
                children[i] = nullptr;
            }
        }
    }

    std::vector<SceneNode*> SceneNode::getAllAttached() {
        // Add itself
        std::vector<SceneNode*> attachedNodes;
        attachedNodes.push_back(this);

        for (auto& child : children) {
            std::vector<SceneNode*> childAttachedNodes = child->getAllAttached();
            attachedNodes.insert(attachedNodes.end(), childAttachedNodes.begin(), childAttachedNodes.end());
        }

        return attachedNodes;
    }

    SceneNode* SceneNode::addChild(SceneNode* child) {
        children.push_back(child);
        child->getTransform()->setParent(&transform);
        child->setEngine(engine);
        return this;
    }
    SceneNode* SceneNode::addComponent(NodeComponent* component) {
        components.push_back(component);
        component->setParent(this);
        return this;
    }

    void SceneNode::removeChild(SceneNode* child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void SceneNode::handleInput(float deltaTime) {
        for (auto& c: components) {
            c->handleInput(deltaTime);
        }
        for (auto& c: children) {
            c->handleInput(deltaTime);
        }
    } void SceneNode::update(float deltaTime) {
        for (auto& c: components) {
            c->update(deltaTime);
        }

        for (auto& c: children) {
            c->update(deltaTime);
        }
    }

    void SceneNode::render(Shader* shader, RenderingEngine* renderingEngine, Camera* camera) {
        for (auto& c: components) {
            c->render(*shader, *renderingEngine, *camera);
        }

        for (auto& c: children) {
            c->render(shader, renderingEngine, camera);
        }
    }

    void SceneNode::setEngine(Engine* e) {
        if (engine != e) {
            engine = e;
            for (auto& c: components) {
                c->addToEngine(e);
            }

            for (auto& c: children) {
                c->setEngine(e);
            }
        }
    }

}
