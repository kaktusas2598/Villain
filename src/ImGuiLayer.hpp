#ifndef __IMGUI_LAYER__
#define __IMGUI_LAYER__

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

        // Draw different tools, can potentially be refactored to new classes
        void drawScene(Engine& engine);
        void drawSceneGraph(Engine& engine);
        void drawSettings(Engine& engine);
        void drawAssetBrowser();
    private:
        void setupDockspace();
        // Render Scene Graph nodes and components recursively
        void drawNode(SceneNode* node);

        static bool showDemoWindow; ///< Toggle IMGui Demo Window for Docs
    };


}

#endif // __IMGUI_LAYER__
