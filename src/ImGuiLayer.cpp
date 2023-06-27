#include "ImGuiLayer.hpp"

#include "Camera.hpp"
#include "DebugConsole.hpp"
#include "ResourceManager.hpp"
#include "SoundManager.hpp"

#include "Application.hpp"
#include "Engine.hpp"
#include "SceneNode.hpp"

// Custom baked fonts for ImGui
#include "imgui/Roboto-Regular.h"

// TODO: refactor scene graph rendering/editing to separate class, to avoid stuff like this
#include "components/CameraComponent.hpp"
#include "components/Light.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/PhysicsObjectComponent.hpp"
#include "imgui/imgui.h"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    bool ImGuiLayer::showDemoWindow = false;
    ImVec4 ImGuiLayer::clearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    ImGuiLayer::ImGuiLayer() {}

    ImGuiLayer::~ImGuiLayer() {}

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
        io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_Regular_compressed_data, Roboto_Regular_compressed_size, 16.0f, &config, io.Fonts->GetGlyphRangesDefault());
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
        style.FrameBorderSize                   = 1;
        style.TabBorderSize                     = 1;
        style.WindowRounding                    = 7;
        style.ChildRounding                     = 4;
        style.FrameRounding                     = 3;
        style.PopupRounding                     = 4;
        style.ScrollbarRounding                 = 9;
        style.GrabRounding                      = 3;
        style.LogSliderDeadzone                 = 4;
        style.TabRounding                       = 4;

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init("#version 330");
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
        drawScene(engine);
        drawSceneGraph(engine);
        drawSettings(engine);
        drawAssetBrowser();
        drawSelectedNode();
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        //TODO: file browser to load assets
        // some kind of scene manager or ECS manager
        // some kind of tool to render stuff with DebugRenderer
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

    void ImGuiLayer::drawMenu() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
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

            //printf("Engine Mouse Coords X: %f Y: %f\n", InputManager::Instance()->getMouseCoords().x, InputManager::Instance()->getMouseCoords().y);
            //printf("ImGui GetCursorScreenPos() X: %f Y: %f\n", sceneViewportPosition.x, sceneViewportPosition.y);
            //printf("ImGui::GetMousePos() X: %f Y: %f\n", imGuiMousePosition.x, imGuiMousePosition.y);
            //printf("Scene image Coords X: %f Y: %f\n", mousePosRelativeToSceneViewport.x, mousePosRelativeToSceneViewport.y);

            ImGui::Image(
                    (ImTextureID)engine.getSceneBuffer()->getTextureID(),
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

    void ImGuiLayer::drawSceneGraph(Engine& engine) {
        ImGui::Begin("Scene Graph");
        {
            ImGui::SetNextItemOpen(true);
            ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
            drawNode(engine.getApplication()->getRootNode());
            ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

            if (ImGui::Button("New Node")) {
                SceneNode* newNode = new SceneNode("testing");
                engine.getApplication()->getRootNode()->addChild(newNode);
            }
        }
        ImGui::End();
    }

    void ImGuiLayer::drawNode(SceneNode* node) {
        // Make sure tree nods fill all available width and disable single click to open behaviour
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (selectedNode == node) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen =  ImGui::TreeNodeEx(node->getName().c_str(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            selectedNode = node;
            // Set id in rendering engine so that color mod is applied to shader when drawing selected node
            node->getEngine()->getRenderingEngine()->setSelectedNodeID(selectedNode->getID());
        }
        if (nodeOpen) {
            if (ImGui::BeginTabBar("NodeProps", ImGuiTabBarFlags_None)) {
                if (ImGui::BeginTabItem("Transform")) {
                    drawNodeProperties(node);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Components")) {
                    drawNodeComponents(node);
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            // Children
            for (auto& child: node->getChildren()) {
                drawNode(child);
            }

            ImGui::TreePop();
        }
    }

    void ImGuiLayer::drawSelectedNode() {
        ImGui::Begin("Active Node");

        if (selectedNode != nullptr) {
            ImGui::Text(selectedNode->getName().c_str());

            drawNodeProperties(selectedNode);

            if (!selectedNode->getComponents().empty()) {
                drawNodeComponents(selectedNode);
            }


            ImGui::Separator();
            const char* componentList[] = {"Camera", "Light", "Basic Mesh", "Model", "Move Controller", "Look Controller", "Physics"};
            static int selectedComponent = 1;
            // NOTE: wrap ListBox() in if condition to check for click events on list items
            ImGui::ListBox("Add Component", &selectedComponent, componentList, IM_ARRAYSIZE(componentList), 4);

            std::vector<VertexP1N1UV> vertices;
            std::vector<unsigned int> indices;
            glm::vec3 diffuseColor = glm::vec3(1.0f);
            glm::vec3 lightDirection = glm::vec3(0.0f);
            float spotLightCutoff = 12.5f;
            float spotLightOuterCutoff = 17.5f;

            // Display component creation info depending on selection in the listbox
            switch (selectedComponent) {
                    case 0:
                        if (ImGui::Button("Add camera")) {
                            selectedNode->addComponent(new CameraComponent(new Camera(ProjectionType::ORTHOGRAPHIC)));
                        }
                        break;
                    case 1:
                        ImGui::ColorEdit3("Diffuse light ", (float*)&diffuseColor);
                        ImGui::SliderFloat3("Direction (for spot and directional lights only)", (float*)&lightDirection, -1.f, 1.f);
                        ImGui::DragFloat("Spot light cutoff angle", &spotLightCutoff, 1.0f, 0.0f, 180.0f);
                        ImGui::DragFloat("Spot light outer cutoff angle", &spotLightOuterCutoff, 1.0f, 0.0f, 180.0f);
                        ImGui::Separator();

                        if (ImGui::Button("Add Point Light")) {
                            selectedNode->addComponent(
                                    new PointLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                        selectedNode->getTransform()->getPos()));
                        } ImGui:: SameLine();
                        if (ImGui::Button("Add Spot Light")) {
                            selectedNode->addComponent(
                                    new SpotLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                        selectedNode->getTransform()->getPos(), lightDirection,
                                        glm::cos(glm::radians(spotLightCutoff)), glm::cos(glm::radians(spotLightOuterCutoff))));

                        } ImGui:: SameLine();
                        if (ImGui::Button("Add Directional Light")) {
                            selectedNode->addComponent(
                                    new DirectionalLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                        lightDirection));
                        } ImGui:: SameLine();

                        break;
                    case 2:
                        // TODO:
                        if (ImGui::Button("Add sphere mesh")) {
                            MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, 1.0f);
                            selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                        }
                        if (ImGui::Button("Add Axis-Aligned Bounding Box mesh")) {
                            MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices);
                            selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                        }
                        break;
                    case 3:
                        // TODO: need a way to select model, possibly using C++'s filesystem header
                        //selectedNode->addComponent(new ModelRenderer(""))
                        break;
                    default:
                        printf("Option %s not implemented\n", componentList[selectedComponent]);
                        break;
                }

        }

        ImGui::End();
    }

    void ImGuiLayer::drawNodeProperties(SceneNode* node) {
        ImGui::DragFloat("Scale", node->getTransform()->getScalePtr(), 1.0f, 0.0f, 10.0f);

        ImGui::Text("Position"); ImGui::SameLine();
        ImGui::PushItemWidth(40);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(1.0f, 0.0f, 0.0f));
        ImGui::DragFloat("X", &node->getTransform()->getPos().x); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.0f, 1.0f, 0.0f));
        ImGui::DragFloat("Y", &node->getTransform()->getPos().y); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.0f, 0.0f, 1.0f));
        ImGui::DragFloat("Z", &node->getTransform()->getPos().z);
        ImGui::PopStyleColor(3);
        ImGui::PopItemWidth();

        ImGui::SliderFloat3("Rot", (float*)&node->getTransform()->getEulerRot(), -360.f, 360.f);
    }

    void ImGuiLayer::drawNodeComponents(SceneNode* node) {
        if (!node->getComponents().empty()) {
            int i = 0;
            for (auto& compo: node->getComponents()) {
                ImGui::PushID(i); // Solves issues with multiple elements sharing same names
                // TODO: find optimal way of adding any components and possibly custom components without too
                // many conditionals
                if (compo->getID() == GetId<CameraComponent>()) {
                    ImGui::Text("Camera");
                    Camera* camera = static_cast<CameraComponent*>(compo)->getCamera();
                    static int projectionType = (int)camera->getProjectionType();
                    ImGui::RadioButton("NONE", &projectionType, 0); ImGui::SameLine();
                    ImGui::RadioButton("ORHTOGRAPHIC", &projectionType, 1); ImGui::SameLine();
                    ImGui::RadioButton("2D", &projectionType, 2); ImGui::SameLine();
                    ImGui::RadioButton("PERSPECTIVE", &projectionType, 3);
                    camera->setProjectionType((ProjectionType)projectionType);
                }
                auto light = dynamic_cast<BaseLight*>(compo);
                if (light != nullptr) {
                    ImGui::Text("%s light", light->type().c_str());
                    ImGui::DragFloat("Shadow Bias", light->getShadowInfo()->getBiasPointer());
                    ImGui::ColorEdit3("Diffuse light color", (float*)&light->DiffuseColor);
                    // HACK: seems like I do this everywhere anyway, so maybe separate properties for specular and ambient colours of light sources
                    // are not needed? yet to be decided
                    light->AmbientColor = light->DiffuseColor * 0.2f;
                }

                auto meshN1UV = dynamic_cast<MeshRenderer<VertexP1N1UV>*>(compo);
                if (meshN1UV != nullptr) {
                    ImGui::Text("Basic Mesh");
                    static int loadedMesh = 0;
                    auto originalMesh = meshN1UV->getMesh();
                    std::vector<VertexP1N1UV> vertices;
                    std::vector<unsigned int> indices;

                    ImGui::RadioButton("DEFAULT", &loadedMesh, 0); ImGui::SameLine();
                    ImGui::RadioButton("SPHERE", &loadedMesh, 1); ImGui::SameLine();
                    ImGui::RadioButton("AABB", &loadedMesh, 2);

                    static float sphereRadius = 1.0f;
                    static float aabbSize = 1.0f;;
                    ImGui::DragFloat("Sphere radius", &sphereRadius, 1.0f, 0.0f, 100.0f, "%.1f");
                    ImGui::DragFloat("AABB size", &aabbSize, 1.0f, 0.0f, 100.0f, "%.1f");

                    if (loadedMesh == 0)
                        meshN1UV->setMesh(originalMesh);
                    else if (loadedMesh == 1) {
                        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, sphereRadius);
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    } else if (loadedMesh == 2) {
                        MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices, glm::vec3(0.0f), glm::vec3(aabbSize/2));
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    }
                }

                auto model = dynamic_cast<ModelRenderer*>(compo);
                if (model != nullptr) {
                    ImGui::Text("Model %s at %s", model->getModel()->getFilename().c_str(), model->getModel()->getDirectory().c_str());
                    if (model->getCurrentAnimation()) {
                        if (ImGui::BeginCombo("Active animation", model->getCurrentAnimation()->getName().c_str())) {
                            for (const auto& anim: model->getModel()->getAnimations())  {
                                bool isSelected = (anim.first == model->getCurrentAnimation()->getName());
                                if (ImGui::Selectable(anim.first.c_str(), isSelected)) {
                                    model->getAnimator()->playAnimation(anim.second);
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::DragFloat("Animation speed", model->getCurrentAnimation()->getSpeed(), 1.0f, 0.1f, 10000.0f, "%.1f");
                        ImGui::DragFloat("Animation time", model->getAnimator()->getCurrentTime(), 1.0f, 0.0f, model->getCurrentAnimation()->getDuration(), "%.1f");
                        ImGui::Checkbox("Bind Pose", model->getAnimator()->getBindPose());
                        for(auto& boneInfo: model->getModel()->getBoneInfoMap()) {
                            if (boneInfo.second.id == model->getModel()->getDisplayedBoneIndex()) {
                                ImGui::Text("Displayed Bone: '%s'", boneInfo.first.c_str());
                            }
                        }
                        if (ImGui::Button("Pause Animation")) {
                            model->getAnimator()->toggleAnimation();
                        }

                    }
                }

                ImGui::Separator();
                ImGui::PopID();
                i++;
            }
        }
    }

    void ImGuiLayer::drawSettings(Engine& engine) {
        static bool showDemoWindow = false;
        ImGui::Begin("Settings");

        // Engine Info and settings
        ImGui::Text("Engine FPS: %.1f ", engine.getFps());
        ImGui::Text("Update frame time: %.1u ms ", engine.getUpdateTime());
        ImGui::Text("Render frame time: %.1u ms", engine.getRenderTime());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Mouse coords(Window): %.1f, %.1f", InputManager::Instance()->getMouseCoords().x, InputManager::Instance()->getMouseCoords().y);

        ImGui::Separator();
        ImGui::ColorEdit4("Screen clear color: ", (float*)&clearColor);
        ImGui::Checkbox("Wireframe mode", engine.wireFrameModeActive());
        ImGui::Checkbox("Visualise normals", engine.getRenderingEngine()->getVisualiseNormals());
        ImGui::Checkbox("Visualise bone weights", engine.getRenderingEngine()->getVisualiseBoneWeights());
        ImGui::Checkbox("Gamma correction enabled(Gamma = 2.2)", engine.getRenderingEngine()->getGammaCorrection());
        ImGui::Checkbox("Toon shading enabled", engine.getRenderingEngine()->getToonShadingEnabled());
        ImGui::Checkbox("Mirror enabled", engine.getRenderingEngine()->getMirrorFramebufferEnabled());
        ImGui::ColorEdit3("Ambient lighting color: ", (float*)engine.getRenderingEngine()->getAmbientLightColor());

        ImGui::Separator();
        ImGui::ColorEdit3("Fog color: ", (float*)engine.getRenderingEngine()->getFogColor());
        ImGui::Checkbox("Exponential fog enable d(defaults to layered fog)", engine.getRenderingEngine()->exponentialFogEnabled());
        ImGui::DragFloat("Fog Density", (float*)engine.getRenderingEngine()->getFogDensity(), 0.0005f, 0.0f, 1.0f, "%.5f");
        ImGui::DragFloat("Fog Gradient", (float*)engine.getRenderingEngine()->getFogGradient(), 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Layered Fog Top", (float*)engine.getRenderingEngine()->getLayeredFogTop(), 1.0f, 0.0f, 1000.0f, "%.1f");
        ImGui::DragFloat("Fog end", (float*)engine.getRenderingEngine()->getLayeredFogEnd(), 1.0, 0.0f, 1000.0f, "%.1f");

        ImGui::Separator();
        ImGui::Text("Post-Processing Effects");
        ImGui::Checkbox("Invert colors", engine.getRenderingEngine()->getInvertColors());
        ImGui::Checkbox("Grayscale", engine.getRenderingEngine()->getGrayScale());
        ImGui::Checkbox("Sharpen", engine.getRenderingEngine()->getSharpen());
        ImGui::Checkbox("Blur", engine.getRenderingEngine()->getBlur());
        ImGui::Checkbox("Edge outline", engine.getRenderingEngine()->getEdgeDetection());
        ImGui::Checkbox("Show IMGui Demo Window", &showDemoWindow);

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

