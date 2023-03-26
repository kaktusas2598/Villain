#include "SceneNode.hpp"

namespace Villain {

    SceneNode::SceneNode() {}

    SceneNode::~SceneNode() {}

    void SceneNode::handleInput() {
        for (auto& c: children) {
            c->handleInput();
        }
    }

    void SceneNode::update(float deltaTime) {
        for (auto& c: children) {
            c->update(deltaTime);
        }
    }

    void SceneNode::render() {
        for (auto& c: children) {
            c->render();
        }
    }

}
