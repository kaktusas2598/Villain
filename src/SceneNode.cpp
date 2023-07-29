#include "SceneNode.hpp"

#include "NodeComponent.hpp"
#include "camera/Camera.hpp"

#include <algorithm> // For std::remove and std::sort

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

    SceneNode* SceneNode::addChild(SceneNode* child) {
        children.push_back(child);
        child->getTransform()->setParent(&transform);
        child->setEngine(engine);
        return this;
    }
    SceneNode* SceneNode::addComponent(NodeComponent* component) {
        components.push_back(component);
        component->setParent(this);
        // Used in case component was added to Node, after that Node was added to Scene Graph!
        if (engine) component->addToEngine(engine);
        return this;
    }

    void SceneNode::removeChild(SceneNode* child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void SceneNode::removeComponent(NodeComponent* component) {
        components.erase(std::remove(components.begin(), components.end(), component), components.end());
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
        std::vector<std::pair<SceneNode*, float>> nodeDistances;
        collectNodeDistances(camera, nodeDistances);

        // Sort from farthest to nearest relative to camera for correct tranparency
        // We assume this will only be called from the root node!
        std::sort(nodeDistances.begin(), nodeDistances.end(),
                [](const std::pair<SceneNode*, float>& a, const std::pair<SceneNode*, float>& b) {
                return a.second > b.second;
                });

        for (const auto& nodeDistance : nodeDistances) {
            SceneNode* node = nodeDistance.first;
            for (auto& component : node->getComponents()) {
                component->render(*shader, *renderingEngine, *camera);
            }
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

    void SceneNode::collectNodeDistances(Camera* camera, std::vector<std::pair<SceneNode*, float>>& nodeDistances) {
        // Calculate the distance from this node to the camera
        float distance = glm::distance(camera->getPosition(), transform.getPos());

        // Add the node and its distance to the vector
        nodeDistances.push_back(std::make_pair(this, distance));

        // Recursively traverse the children nodes
        for (auto& child : children) {
            child->collectNodeDistances(camera, nodeDistances);
        }
    }
}
