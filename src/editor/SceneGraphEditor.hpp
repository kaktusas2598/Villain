#pragma once

#include "EventDispatcher.hpp"
#include "SceneNode.hpp"

namespace Villain {

    class ImGuiLayer;

    class SceneGraphEditor {
        public:
            SceneGraphEditor(ImGuiLayer* editor) : editor(editor) {}

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

            std::vector<std::string> supportedModelFormats = {".obj", ".dae", ".fbx", ".gltf", ".glb"};
            std::vector<std::string> supportedTextureFormats = {"bmp", ".jpg", ".jpeg", ".png", "tga"};
    };
}
