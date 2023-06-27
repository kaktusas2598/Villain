#ifndef __NODE_COMPONENT__
#define __NODE_COMPONENT__

#include "SceneNode.hpp"
#include "Camera.hpp"
#include <unordered_map>
#include <typeindex>

namespace Villain {



    // Responsible for generating and managing unique component type identifiers
    class ComponentTypeRegistry {
    public:
        template <typename T>
        static int getTypeID() {
            static const int typeID = nextTypeID++;
            return typeID;
        }

    private:
        static int nextTypeID;
    };


    // Must be used once in each component class header to register unique id!
#define DECLARE_COMPONENT_TYPE(componentClass) \
    static int getTypeID() { \
        static const int typeID = ComponentTypeRegistry::getTypeID<componentClass>(); \
        return typeID; \
    }

    // Component signature idea borrowed from ECS designs
    // Generate unique ID for every new component, eg:
    // int id = GetId<CameraComponent>();
    //
    //extern int componentCounter;
    //template <class T> int GetId() {
        //static int componentId = componentCounter++;
        //return componentId;
    //}

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

            int getID() const { return id; }
            inline SceneNode* getParent() { return parent; }
            virtual void setParent(SceneNode* p) { parent = p; }
        protected:
            SceneNode* parent;
            int id = -1;
        private:
            NodeComponent(const NodeComponent& other) {}
            void operator=(const NodeComponent& other) {}
    };
}

#endif // __NODE_COMPONENT__
