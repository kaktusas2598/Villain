#ifndef __NODE_COMPONENT__
#define __NODE_COMPONENT__

#include "SceneNode.hpp"
#include "Camera.hpp"

namespace Villain {
    class RenderingEngine;
    class Shader;

    // Defines logic for Scene Node
    // Kind of like ECS components and systems combined, not ideal approach but wayyy beter
    // than inheritance approach
    class NodeComponent {
        public:
            NodeComponent() :
                parent(0) {}
            virtual ~NodeComponent() {}

            virtual void handleInput(float deltaTime) {}
            virtual void update(float deltaTime) {}
            virtual void render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {}

            virtual void addToEngine(Engine* engine) { }

            inline Transform* GetTransform()             { return parent->getTransform(); }
            inline const Transform& GetTransform() const { return *parent->getTransform(); }

            inline const SceneNode* getParent() { return parent; }
            virtual void setParent(SceneNode* p) { parent = p; }
        protected:
            SceneNode* parent;
        private:

            NodeComponent(const NodeComponent& other) {}
            void operator=(const NodeComponent& other) {}
    };
}

#endif // __NODE_COMPONENT__
