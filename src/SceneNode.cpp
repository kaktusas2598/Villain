#include "SceneNode.hpp"

#include "NodeComponent.hpp"

namespace Villain {

    SceneNode::SceneNode() {
        transform.Position = glm::vec3(0.0f);
        transform.Rotation = glm::vec3(0.0f);
        transform.Scale = 1.0f;
    }

    SceneNode::~SceneNode() {
        for (unsigned int i = 0; i < components.size(); i++) {
            if (components[i]) {
                delete components[i];
            }
        }

        for(unsigned int i = 0; i < children.size(); i++) {
            if (children[i]) {
                delete children[i];
            }
        }
    }

    SceneNode* SceneNode::addChild(SceneNode* child) {
        children.push_back(child);
        //child->getTransform()->setParent(transform); // TODO: set up scene graph transform hierarchy
        child->setEngine(engine);
        return this;
    }
    SceneNode* SceneNode::addComponent(NodeComponent* component) {
        components.push_back(component);
        component->setParent(this);
        return this;
    }


    void SceneNode::handleInput() {
        for (auto& c: components) {
            c->handleInput(0.0f);//TODO: do I need deltatime here at all?
        }
        for (auto& c: children) {
            c->handleInput();
        }
    }

    void SceneNode::update(float deltaTime) {
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