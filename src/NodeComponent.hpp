#pragma once

#include "SceneNode.hpp"


/// Generates unique id per component class, usually called in constructor
#define VILLAIN_SET_COMPONENT_ID(ComponentClass) \
    id = GetId<ComponentClass>()

namespace Villain {

    /// Component signature idea borrowed from ECS designs
    /// Generate unique ID for every new component, eg:
    /// int id = GetId<CameraComponent>();
    extern int componentCounter;
    template <class T> int GetId() {
        static int componentId = componentCounter++;
        return componentId;
    }

    class RenderingEngine;
    class Shader;

    /*! \brief Component for Scene Graph node
     *
     * Lets define custom data and logic and attach it to any scene graph node. Each scene graph node contains
     * a collection of node components, basically Composition approach, but could evolve to ECS in the future
     */
    class NodeComponent {
        public:
            NodeComponent() : parent(nullptr) {}
            virtual ~NodeComponent() {}

            /// Each component can optionally implement logic to handle events
            virtual void handleInput(float deltaTime) {}
            /// Each component can optionally implement update logic
            virtual void update(float deltaTime) {}
            /// Each component can optionally implement render logic
            virtual void render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {}

            // Will be called after node containing this component gets added to scene graph or if we
            // add this component to node after that node was already added to scene graph
            virtual void addToEngine(Engine* engine) { }

            /// Transform will be calculated by traversing scene graph hierarchy up to root node
            inline Transform* GetTransform()             { return parent->getTransform(); }
            inline const Transform& GetTransform() const { return *parent->getTransform(); }

            /// Get identifier, unique to each type of component
            int getID() const { return id; }
            /// Get scene graph node which owns this component
            inline SceneNode* getParent() { return parent; }
            /// Set owner scene graph node of this parent, called automatically
            virtual void setParent(SceneNode* p) { parent = p; }
        protected:
            SceneNode* parent; ///< Scene graph node which has ownership of this component
            int id = -1; ///< Identifier, unique to each type of component
        private:
            NodeComponent(const NodeComponent& other) {}
            void operator=(const NodeComponent& other) {}
    };
}
