#ifndef __NODE_COMPONENT__
#define __NODE_COMPONENT__

#include "SceneNode.hpp"
#include "Camera.hpp"

namespace Villain {
    class RenderingEngine;
    class Shader;

    //TODO: finish implementing

    // Defines logic for Scene Node
    // Kind of like ECS components and systems combined, not ideal approach but wayyy beter
    // than inheritance approach
    // TODO:
    class NodeComponent {
        public:
            NodeComponent() :
                parent(0) {}
            virtual ~NodeComponent() {}

            virtual void handleInput(float deltaTime) {}
            virtual void update(float deltaTime) {}
            virtual void render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const {}

            virtual void addToEngine(Engine* engine) const { }

            //inline Transform* GetTransform()             { return m_parent->GetTransform(); }
            //inline const Transform& GetTransform() const { return *m_parent->GetTransform(); }

            virtual void setParent(SceneNode* p) { parent = p; }
        private:
            SceneNode* parent;

            NodeComponent(const NodeComponent& other) {}
            void operator=(const NodeComponent& other) {}
    };
}

#endif // __NODE_COMPONENT__
