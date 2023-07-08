#pragma once

#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <filesystem>

#include "AssetBrowser.hpp"
#include "SceneGraphEditor.hpp"
namespace fs = std::filesystem;

namespace Villain {

    class Engine;
    class Window;

    class ImGuiLayer {
    public:
        // Sets up ImGui context and backends
        void init(Window& window);
        // Shutdown ImGui
        void exit();

        // Call before render() to start a new frame
        void start();
        // Call after render() to actually render and end frame
        void end();

        void render(Engine& engine);

        glm::vec2 getSceneViewportPosition() const { return sceneViewportPosition; }
        glm::vec2 getMousePositionRelativeToScene() const { return mousePosRelativeToSceneViewport; }
        float getSceneViewportWidth() const { return sceneViewportWidth; }
        float getSceneViewportHeight() const { return sceneViewportHeight; }

        SceneGraphEditor& getSceneEditor() { return sceneEditor; }

        // Draw different tools, can potentially be refactored to new classes
        void drawMenu();
        void drawScene(Engine& engine);
        void drawSettings(Engine& engine);
        // TODO: refactor into new class
        void drawFileBrowser();
        void drawFileBrowserPath(const fs::path& currentPath);

        // Render font awesome 6 icon, code point example - "\uf07c" for folder open icon
        void renderIcon(const std::string& codePoint, float scale = 1.0f);
    private:
        void setupDockspace();

        static bool showDemoWindow; //<<< Toggle IMGui Demo Window for Docs
        static bool openScenePopup;
        static ImVec4 clearColor;

        float sceneViewportWidth = 0, sceneViewportHeight = 0;
        glm::vec2 sceneViewportPosition{0.0f};
        glm::vec2 mousePosRelativeToSceneViewport{0.0f};

        SceneGraphEditor sceneEditor;
        AssetBrowser assetBrowser;

        ImFont* fontAwesome6;
        ImFont* robotoFont;
    };
}
