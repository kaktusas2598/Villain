#include "ImGuiLayer.hpp"

#include "DebugConsole.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"

#include "Engine.hpp"

namespace Villain {

    bool ImGuiLayer::showDemoWindow = false;

    ImGuiLayer::ImGuiLayer() {}

    ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::init(Window& window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
                                                                    //
                                                                    // Handle cursor visibility ourselves
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void ImGuiLayer::setupDockspace() {
        // DOCKSPACE Setup
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::SetNextWindowBgAlpha(0.0f); // For docking
        ImGui::Begin("DockSpace Demo", NULL, window_flags);

        ImGui::PopStyleVar(2);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        // NOTE: temporary
        //if (ImGui::BeginMainMenuBar()) {
            //if (ImGui::BeginMenu("File")) {
                //if (ImGui::MenuItem("Import")) {
                    ////isImportClicked = true;
                //}
                //ImGui::EndMenu();
            //}
            //ImGui::EndMainMenuBar();
        //}

        ImGui::End();
    }

    void ImGuiLayer::start() {
        // These commands probably should go just before update() method so that states can setup their own ui
        // Start the Dear ImGui frame?
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::render(Engine& engine) {
        // NOTE: Must be done after starting new frame and before any Imgui rendering is done!
        setupDockspace();

        // Draw all different parts of the toolkit
        DebugConsole::Instance()->render();
        drawScene(engine);
        drawSettings(engine);
        drawAssetBrowser();
        //TODO: file browser to load assets
        // some kind of scene manager or ECS manager
        // some kind of tool to render stuff with DebugRenderer
        // ability to manage lights camera and everything
    }

    void ImGuiLayer::end() {
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        ImGui::EndFrame();
    }

    void ImGuiLayer::drawScene(Engine& engine) {
        ImGui::Begin("Scene");
        {
            ImGui::BeginChild("GameRender");

            float width = ImGui::GetContentRegionAvail().x;
            float height = ImGui::GetContentRegionAvail().y;

            ImGui::Image(
                    (ImTextureID)engine.getSceneBuffer()->getTextureID(),
                    //ImGui::GetContentRegionAvail(),
                    ImGui::GetWindowSize(),
                    ImVec2(0, 1),
                    ImVec2(1, 0)
                    );
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void ImGuiLayer::drawSettings(Engine& engine) {
        static bool showDemoWindow = false;
        ImGui::Begin("Settings");

        // Engine Info
        ImGui::Text("Engine FPS: %.1f ", engine.getFps());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Mouse coords(Window): %.1f, %.1f", InputManager::Instance()->getMouseCoords().x, InputManager::Instance()->getMouseCoords().y);
        ImGui::Checkbox("Show IMGui Demo Window", &showDemoWindow);
        //TODO:clear color
        //ImGui::ColorEdit4("Screen clear color: ", (float*)&clearColor);
        ImGui::Separator();

        // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);
    }

    void ImGuiLayer::drawAssetBrowser() {
        ImGui::Begin("Asset Browser");
        {
            if (ImGui::TreeNode("Assets")) {
                if (ImGui::TreeNode("Music")) {
                    for (auto const& t: SoundManager::Instance()->getMusicMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            SoundManager::Instance()->playMusic(t.first);

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Shaders")) {
                    for (auto const& t: ResourceManager::Instance()->getShaderMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Sound FX")) {
                    for (auto const& t: SoundManager::Instance()->getSoundFXMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            SoundManager::Instance()->playSound(t.first);

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Textures")) {
                    for (auto const& t: ResourceManager::Instance()->getTextureMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            float width = ImGui::GetContentRegionAvail().x;
                            float height = ImGui::GetContentRegionAvail().y;

                            ImGui::Image(
                                    (ImTextureID)t.second->getID(),
                                    //ImGui::GetWindowSize(), // will respect aspect ratio of image
                                    ImGui::GetContentRegionAvail(), // will squish image to fit it in
                                    ImVec2(0, 1),
                                    ImVec2(1, 0)
                                    );

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                // TODO: fonts, levels

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}

