#ifndef __IMGUI_LAYER__
#define __IMGUI_LAYER__

#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

namespace Villain {

    class Engine;
    class SceneNode;
    class Window;

    class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

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

        void setSelectedNode(SceneNode* node) { selectedNode = node; }

        // Draw different tools, can potentially be refactored to new classes
        void drawMenu();
        void drawScene(Engine& engine);
        void drawSceneGraph(Engine& engine);
        void drawSettings(Engine& engine);
        void drawAssetBrowser();
    private:
        void setupDockspace();
        void drawNode(SceneNode* node); //<<< Render Scene Graph nodes and components recursively
        void drawSelectedNode();

        void drawNodeProperties(SceneNode* node);
        void drawNodeComponents(SceneNode* node);

        static bool showDemoWindow; //<<< Toggle IMGui Demo Window for Docs

        float sceneViewportWidth = 0, sceneViewportHeight = 0;
        glm::vec2 sceneViewportPosition{0.0f};
        glm::vec2 mousePosRelativeToSceneViewport{0.0f};

        SceneNode* selectedNode = nullptr;
    };
}

#endif // __IMGUI_LAYER__
