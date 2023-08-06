#pragma once

#include "EventDispatcher.hpp"
#include "ResourceManager.hpp"
#include "SceneNode.hpp"

namespace Villain {

    class ImGuiLayer;

    class SceneGraphEditor {
        public:
            SceneGraphEditor(ImGuiLayer* editor);

            void init();
            void render(Engine& engine);
            void onEvent(Event& event);

            void setSelectedNode(SceneNode* node) { selectedNode = node; }

        private:
            void drawNodeHierarchy(SceneNode* node); //<<< Render Scene Graph nodes and components recursively
            void drawSelectedNode();

            void drawNodeProperties(SceneNode* node);
            void drawNodeComponents(SceneNode* node);

            ImGuiLayer* editor = nullptr;
            SceneNode* selectedNode = nullptr;

            enum class ComponentType { Camera, Light, Mesh, Model, MoveController, LookController, RigidBody, Script, None };
            std::map<ComponentType, const char*> componentNames = {
                { ComponentType::None, "None" },
                { ComponentType::Camera, "Camera" },
                { ComponentType::Light, "Light" },
                { ComponentType::Mesh, "Mesh" },
                { ComponentType::Model, "Model" },
                { ComponentType::MoveController, "Move Controller" },
                { ComponentType::LookController, "Look Controller" },
                { ComponentType::RigidBody, "Rigid Body" },
                { ComponentType::Script, "Script" }
            };
            ComponentType selectedComponent = ComponentType::None;

            std::vector<std::string> supportedModelFormats = {".obj", ".dae", ".fbx", ".gltf", ".glb"};
            std::vector<std::string> supportedTextureFormats = {"bmp", ".jpg", ".jpeg", ".png", "tga"};
    };
}
