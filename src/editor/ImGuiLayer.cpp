#include "ImGuiLayer.hpp"

#include "Application.hpp"
#include "DebugConsole.hpp"
#include "Engine.hpp"
#include "ResourceManager.hpp"

// Custom baked fonts for ImGui;
#include "imgui/fontawesome6.h"
#include "imgui/Roboto-Regular.h"
#include "imgui/source_sans_pro_regular.h"

namespace Villain {

    bool ImGuiLayer::showDemoWindow = false;
    ImVec4 ImGuiLayer::clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    ImGuiLayer::ImGuiLayer(Engine* e)
        : engine(e), assetBrowser(this), fileBrowser(this), sceneEditor(this) {}

    void ImGuiLayer::exit() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::init(Window& window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Load custom font
        //io.Fonts->AddFontDefault();
        // ImGui searches in working directory
        //io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", 14.0f);
        // Add custom fonts built by ImGui's binary_to_compressed_c script!
        ImFontConfig config;
        config.PixelSnapH = true;
        //robotoFont = io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_Regular_compressed_data, Roboto_Regular_compressed_size, 16.0f, &config, io.Fonts->GetGlyphRangesDefault());
        ImFontGlyphRangesBuilder glyphRangeBuilder;
        glyphRangeBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        robotoFont = io.Fonts->AddFontFromMemoryCompressedTTF(source_sans_pro_regular_compressed_data, source_sans_pro_regular_compressed_size, 21.0f, &config, io.Fonts->GetGlyphRangesDefault());
        ImWchar rangesFontAwesome[] = { 0xF000, 0xF8FF, 0 };
        fontAwesome6 = io.Fonts->AddFontFromMemoryCompressedTTF(fontawesome6_compressed_data, fontawesome6_compressed_size, 16.0f, &config, rangesFontAwesome);
        io.Fonts->Build();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;      // Handle cursor visibility ourselves

        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();

        // Credits to https://github.com/ocornut/imgui/issues/707 for custom themes for imgui
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding                     = ImVec2(8.00f, 8.00f);
        style.FramePadding                      = ImVec2(5.00f, 2.00f);
        style.CellPadding                       = ImVec2(6.00f, 6.00f);
        style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
        style.IndentSpacing                     = 25;
        style.ScrollbarSize                     = 15;
        style.GrabMinSize                       = 10;
        style.WindowBorderSize                  = 1;
        style.ChildBorderSize                   = 1;
        style.PopupBorderSize                   = 1;
        style.FrameBorderSize                   = 2;
        style.TabBorderSize                     = 1;
        style.WindowRounding                    = 7;
        style.ChildRounding                     = 4;
        style.FrameRounding                     = 3;
        style.PopupRounding                     = 2;
        style.ScrollbarRounding                 = 9;
        style.GrabRounding                      = 3;
        style.LogSliderDeadzone                 = 4;
        style.TabRounding                       = 4;

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init("#version 330");

        sceneEditor.init();
    }

    void ImGuiLayer::setupDockspace() {
        // DOCKSPACE Setup
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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


        ImGui::End();
    }

    void ImGuiLayer::processInput(SDL_Event* event) {
        ImGui_ImplSDL2_ProcessEvent(event);

        // Editor shortcuts
        if (Input::Get()->isKeyDown(SDLK_LCTRL) && Input::Get()->isKeyDown(SDLK_o))
            fileBrowser.openPopup(".xml");
        if (Input::Get()->isKeyDown(SDLK_LCTRL) && Input::Get()->isKeyDown(SDLK_s))
            fileBrowser.savePopup(".xml");
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
        drawMenu();
        DebugConsole::Instance()->render();
        fileBrowser.render();
        assetBrowser.render();
        sceneEditor.render(engine);
        drawSettings(engine);
        scriptEditor.render();
        drawScene(engine);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
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

    void ImGuiLayer::renderIcon(const std::string& codePoint, float scale) {
        // Icon test
        fontAwesome6->Scale = scale;
        ImGui::PushFont(fontAwesome6);
        ImGui::Text("%s", codePoint.c_str());
        ImGui::PopFont();
        fontAwesome6->Scale = 1.0f;
    }

    void ImGuiLayer::drawMenu() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Scene")) {
                    fileBrowser.openPopup(".xml");
                }
                ImGui::SameLine(); renderIcon("\uf07c");

                if (ImGui::MenuItem("Save Scene")) {
                    fileBrowser.savePopup(".xml");
                }
                ImGui::SameLine(); renderIcon("\uf0c7");

                if (ImGui::MenuItem("Quit")) {
                    Engine::setRunning(false);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) {
                if (ImGui::BeginMenu("Post Processing Effects")) {
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void ImGuiLayer::drawScene(Engine& engine) {
        // Remove any padding around scene view window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Scene");
        {
            ImGui::BeginChild("GameRender");

            // Get available area size to display scene viewport onto
            sceneViewportWidth = ImGui::GetContentRegionAvail().x;
            sceneViewportHeight = ImGui::GetContentRegionAvail().y;

            // Rescale scene viewport image to be same aspect ratio as engine's aspect ratio
            //float oldWidth = sceneViewportWidth, oldHeight = sceneViewportHeight;
            // NOTE: Maybe alternatively we could change engine's aspect ratio (including's scene buffer)
            // to make sure, inside editor, image fills up all available region, TODO?
            float engineAspectRatio = Engine::getScreenWidth() / (float)Engine::getScreenHeight();
            if (sceneViewportHeight >= sceneViewportWidth) {
                sceneViewportHeight = sceneViewportWidth / engineAspectRatio;
                // Scale image back up proportionally to fill viewport - causes issues, viewpoint changes,
                // possible solution would be to introduce editor camera
                //float ratio = oldHeight / sceneViewportHeight;
                //sceneViewportHeight *= ratio;
                //sceneViewportWidth *= ratio;
            } else {
                sceneViewportWidth = sceneViewportHeight * engineAspectRatio;
                //float ratio = oldWidth / sceneViewportWidth;
                //sceneViewportHeight *= ratio;
                //sceneViewportWidth *= ratio;
            }

            //printf("Engine width: %d height: %d\n", Engine::getScreenWidth(), Engine::getScreenHeight());
            //printf("Scene vieport(image only) width: %f height: %f\n", sceneViewportWidth, sceneViewportHeight);

            // NOTE: https://github.com/ocornut/imgui/issues/3404
            ImVec2 imGuiMousePosition = ImGui::GetMousePos(); // Global ImGui mouse coordinates, used with multiple viewports
            // Global ImGui scene framebuffer window position, must be called before ImGui::Image()
            sceneViewportPosition = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

            mousePosRelativeToSceneViewport = glm::vec2(imGuiMousePosition.x, imGuiMousePosition.y) - sceneViewportPosition;

            //printf("Engine Mouse Coords X: %f Y: %f\n", Input::Get()->getMouseCoords().x, Input::Get()->getMouseCoords().y);
            //printf("ImGui GetCursorScreenPos() X: %f Y: %f\n", sceneViewportPosition.x, sceneViewportPosition.y);
            //printf("ImGui::GetMousePos() X: %f Y: %f\n", imGuiMousePosition.x, imGuiMousePosition.y);
            //printf("Scene image Coords X: %f Y: %f\n", mousePosRelativeToSceneViewport.x, mousePosRelativeToSceneViewport.y);

            ImGui::Image(
                    reinterpret_cast<ImTextureID>(engine.getSceneBuffer()->getTextureID()),
                    //ImGui::GetContentRegionAvail(), // Fill in full available size (scews aspect ratio)
                    ImVec2(sceneViewportWidth, sceneViewportHeight), // Use new scaled viewport size to
                    //ImGui::GetWindowSize(),
                    //ImVec2(Engine::getScreenWidth(), Engine::getScreenHeight()),
                    ImVec2(0, 1),
                    ImVec2(1, 0)
                    );
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ImGuiLayer::drawSettings(Engine& engine) {
        static bool showDemoWindow = false;
        ImGui::Begin("Settings");

        separatorHeader("Engine Info", 1.15);
        ImGui::Text("Engine FPS: %.1f ", engine.getFps());
        ImGui::Text("Update frame time: %.1u ms ", engine.getUpdateTime());
        ImGui::Text("Render frame time: %.1u ms", engine.getRenderTime());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Mouse coords(Window): %.1f, %.1f", Input::Get()->getMouseCoords().x, Input::Get()->getMouseCoords().y);

        auto renderingEngine = engine.getRenderingEngine();
        separatorHeader("Rendering settings", 1.15);
        ImGui::ColorEdit4("Screen clear color: ", (float*)&clearColor);
        ImGui::Checkbox("Wireframe mode", renderingEngine->wireFrameModeActive());
        ImGui::Checkbox("Visualise normals", renderingEngine->getVisualiseNormals());
        ImGui::Checkbox("Visualise bone weights", renderingEngine->getVisualiseBoneWeights());
        ImGui::Checkbox("Gamma correction enabled(Gamma = 2.2)", renderingEngine->getGammaCorrection());
        ImGui::Checkbox("Toon shading enabled", renderingEngine->getToonShadingEnabled());
        ImGui::Checkbox("Mirror enabled", renderingEngine->getMirrorFramebufferEnabled());
        ImGui::ColorEdit3("Ambient lighting color: ", (float*)renderingEngine->getAmbientLightColor());
        ImGui::Checkbox("Show IMGui Demo Window", &showDemoWindow);

        // TODO: Possibly move this to anotehr class or maybe all of this method, not sure yet
        ///////////////////////////////////////////////////
        separatorHeader("Physics settings", 1.15);
        ImGui::Checkbox("Pause Simulation", engine.getRigidBodyWorld()->isSimulationPaused());
        ImGui::Checkbox("Rigid Body Debug Mode", engine.getRigidBodyWorld()->debugModeActive());

        // Draw select list of all colliders currently in rigid body world
        RigidBodyWorld::Colliders& colliders = engine.getRigidBodyWorld()->getColliders();
        static CollisionPrimitive* selectedCollider = nullptr;
        if (ImGui::BeginCombo("Select Collider", selectedCollider ? selectedCollider->getName().c_str() : nullptr)) {
            for (CollisionPrimitive* collider : colliders) {
                bool isSelected = (selectedCollider == collider);
                if (ImGui::Selectable(collider->getName().c_str(), isSelected)) {
                    selectedCollider = collider;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Edit selected collider
        if (selectedCollider) {
            ImGui::PushID(selectedCollider);
            if (auto sphereCollider = dynamic_cast<CollisionSphere*>(selectedCollider)) {
                // Display sphere collider properties
                ImGui::InputFloat("Sphere Radius", &sphereCollider->radius);
            } else if (auto boxCollider = dynamic_cast<CollisionBox*>(selectedCollider)) {
                // Display box collider properties
                ImGui::DragFloat3("OBB Half Size", glm::value_ptr(boxCollider->halfSize));
            } else if (auto planeCollider = dynamic_cast<CollisionPlane*>(selectedCollider)) {
                // Display plane collider properties
                ImGui::InputFloat("Plane Offset", &planeCollider->offset);
                ImGui::DragFloat3("Plane Direction", glm::value_ptr(planeCollider->direction));
            }
            ImGui::DragFloat3("Translation", &selectedCollider->offset[3][0]);
            // TODO: Add ability to attach/detach rigid bodies by selecting them from another combo or sth like that
            ImGui::PopID();
        }

        if (ImGui::Button("Add new collider")) {
            ImGui::OpenPopup("Add Collider Popup");
        }

        // Popup for adding a new collider
        if (ImGui::BeginPopup("Add Collider Popup")) {
            if (ImGui::Selectable("Sphere")) {
                colliders.push_back(new CollisionSphere(1.0f, nullptr));
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Selectable("Box")) {
                colliders.push_back(new CollisionBox({0.5, 0.5, 0.5}, nullptr));
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Selectable("Plane")) {
                colliders.push_back(new CollisionPlane({0, 1, 0}, 0));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ///////////////////////////////////////////////////

        // Fog Settings
        separatorHeader("Fog settings", 1.15);
        ImGui::ColorEdit3("Fog color: ", (float*)renderingEngine->getFogColor());
        ImGui::Checkbox("Exponential fog enable d(defaults to layered fog)", renderingEngine->exponentialFogEnabled());
        ImGui::DragFloat("Fog Density", (float*)renderingEngine->getFogDensity(), 0.0005f, 0.0f, 1.0f, "%.5f");
        ImGui::DragFloat("Fog Gradient", (float*)renderingEngine->getFogGradient(), 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Layered Fog Top", (float*)renderingEngine->getLayeredFogTop(), 1.0f, 0.0f, 1000.0f, "%.1f");
        ImGui::DragFloat("Fog end", (float*)renderingEngine->getLayeredFogEnd(), 1.0, 0.0f, 1000.0f, "%.1f");

        // Enable post processing filters
        separatorHeader("Post-Processing effects", 1.15);
        ImGui::Checkbox("Invert colors", renderingEngine->getInvertColors());
        ImGui::Checkbox("Grayscale", renderingEngine->getGrayScale());
        ImGui::Checkbox("Sharpen", renderingEngine->getSharpen());
        ImGui::Checkbox("Blur", renderingEngine->getBlur());
        ImGui::Checkbox("Edge outline", renderingEngine->getEdgeDetection());

        // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);
    }

    void ImGuiLayer::separatorHeader(const char* text, float fontScale) {
        ImGui::SetWindowFontScale(fontScale);
        ImGui::SeparatorText(text);
        ImGui::SetWindowFontScale(1.0);
    }
}
