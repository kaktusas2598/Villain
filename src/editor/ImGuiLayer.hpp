#pragma once

#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include "AssetBrowser.hpp"
#include "FileBrowser.hpp"
#include "SceneGraphEditor.hpp"
#include "editor/ScriptEditor.hpp"

namespace Villain {

    class Engine;
    class Window;

    /// Responsible for initialising imgui and rendering Villain editor elements
    class ImGuiLayer {
    public:
        ImGuiLayer(Engine* e);

        // Sets up ImGui context and backends
        void init(Window& window);
        // Shutdown ImGui
        void exit();

        /// Forward SDL events to nuklear
        void processInput(SDL_Event* event);

        /// Call before render() to start a new frame
        void start();
        /// Call after render() to actually render and end frame
        void end();

        /// Main imgui editor render routine
        void render(Engine& engine);

        glm::vec2 getSceneViewportPosition() const { return sceneViewportPosition; }
        glm::vec2 getMousePositionRelativeToScene() const { return mousePosRelativeToSceneViewport; }
        float getSceneViewportWidth() const { return sceneViewportWidth; }
        float getSceneViewportHeight() const { return sceneViewportHeight; }

        SceneGraphEditor& getSceneEditor() { return sceneEditor; }
        ScriptEditor& getScriptEditor() { return scriptEditor; }
        AssetBrowser& getAssetBrowser() { return assetBrowser; }
        FileBrowser& getFileBrowser() { return fileBrowser; }
        Engine* getEngine() { return engine; }

        // Draw different tools, can potentially be refactored to new classes
        void drawMenu();
        void drawScene(Engine& engine);
        void drawSettings(Engine& engine);

        // Render font awesome 6 icon, code point example - "\uf07c" for folder open icon
        void renderIcon(const std::string& codePoint, float scale = 1.0f);
        // Render custom size separator text for headers
        void separatorHeader(const char* text, float fontScale);
    private:
        void setupDockspace();

        static bool showDemoWindow; //<<< Toggle IMGui Demo Window for Docs
        static ImVec4 clearColor;

        Engine* engine = nullptr;

        float sceneViewportWidth = 0, sceneViewportHeight = 0;
        glm::vec2 sceneViewportPosition{0.0f};
        glm::vec2 mousePosRelativeToSceneViewport{0.0f};

        SceneGraphEditor sceneEditor;
        AssetBrowser assetBrowser;
        FileBrowser fileBrowser;
        ScriptEditor scriptEditor;

        ImFont* fontAwesome6;
        ImFont* robotoFont;
    };
}
