#include "Engine.hpp"
#include "FrameLimiter.hpp"
#include "ErrorHandler.hpp"
//#include "EntityManager.hpp"
#include "ResourceManager.hpp"

#include "StateParser.hpp"
#include "IGameScreen.hpp"

#include <string>
#include <cstdio> // For sprintf
#include "Logger.hpp"

//#include "ParticleSystem.hpp"
//#include "Collision.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#include "glm/gtc/matrix_transform.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// NOTE: temporary nuklear defines
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

namespace Villain {

    int Engine::screenWidth;
    int Engine::screenHeight;

    int Engine::getScreenWidth() { return screenWidth; }
    int Engine::getScreenHeight() { return screenHeight; }

    Engine::Engine() {
        // Initialize State Machine
        Logger::Instance()->info("Initialising the engine.");
        stateMachine = std::make_unique<StateMachine>(this);
        // Set default level
        //level = nullptr;
    }

    Engine::~Engine() {}

    void Engine::addStates() {
        Logger::Instance()->info("Adding states.");
        StateParser stateParser;
        std::vector<IGameScreen*> states;
        if (!stateParser.loadStates("state.xml", &states)) {
            Logger::Instance()->error("Failed adding states.");
        }
        for (auto& state: states) {
            stateMachine->addScreen(state);
        }
        if (states.size() > 0)
            stateMachine->setScreen(states[0]->getID());
        states.clear();

        return;
    }

    void Engine::onExit() {
    }

    void Engine::init(std::string title, int height, int width, unsigned int currentFlags, bool sdlEnabled){
        screenHeight = height;
        screenWidth = width;

        // Initialize SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            exitWithError("Could Not Initialize SDL.");
        }

        //Initialize PNG loading
        // int imgFlags = IMG_INIT_PNG;
        //if ( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        //exitWithError(IMG_GetError());
        //}

        if( TTF_Init() == -1 ) {
            exitWithError(TTF_GetError());
        }

        // set up a double buffered window (minimizes flickering)
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

        window.create(title, screenHeight, screenWidth, currentFlags);

        nuklearContext = nk_sdl_init(window.getSDLWindow());
        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        {
            struct nk_font_atlas *atlas;
            nk_sdl_font_stash_begin(&atlas);
            /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
            /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
            /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
            /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
            /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
            /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
            nk_sdl_font_stash_end();
            /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
            /*nk_style_set_font(ctx, &roboto->handle);*/
        }

        sceneBuffer = std::make_unique<FrameBuffer>(screenWidth, screenHeight);

        //initialize the current game
        onInit();

        //initialize game screens and add them to the screenList
        addStates();

        //set the MainGame's current game screen
        currentState = stateMachine->getCurrentScreen();

        if (currentState != nullptr) {
            //initialize game screen elements
            currentState->onEntry();
            //set the initial game screen to ScreenState::RUNNING
            currentState->setRunning();
        }

        isRunning = true;//start main loop
    }

    void Engine::run() {

        const float DESIRED_FPS = 60.0f;
        //maximum steps, used to avoid the 'spiral of death'
        const int MAX_PHYSICS_STEPS = 6;
        //set max delta time to avoid speedup
        const float MAX_DELTA_TIME = 1.0f;
        //milliseconds per second
        const float MS_PER_SECOND = 1000;
        //the fps we want in ms / frame
        const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;

        //set max fps to 60
        FrameLimiter frameLimiter;
        frameLimiter.setMaxFPS(DESIRED_FPS);

        //starting time
        Uint32 prevTicks = SDL_GetTicks();

        while (isRunning) {
            frameLimiter.begin();

            //time at the start of the frame
            Uint32 newTicks = SDL_GetTicks();
            //total time the frame took
            Uint32 frameTime = SDL_GetTicks() - prevTicks;
            //update previous frame time
            prevTicks = newTicks;

            //how much extra time we have in the frame
            float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

            // input manager update used to be here

            //SEMI FIXED TIME STEP ??
            int updateCount = 0;
            while (totalDeltaTime > 0.0f && updateCount < MAX_PHYSICS_STEPS && isRunning) {
                //update input manager
                TheInputManager::Instance()->update();
                SDL_Event event;
                nk_input_begin(nuklearContext);
                while (SDL_PollEvent(&event)) {
                    ImGui_ImplSDL2_ProcessEvent(&event);
                    handleEvents(event);
                    nk_sdl_handle_event(&event);
                }
                nk_input_end(nuklearContext);

                //limit deltatime to 1.0 so no speedup (1.0 being one frame and .2 being a fifth of a frame)
                float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);

                deltaTime = deltaTime / DESIRED_FPS;

                onAppPreUpdate(deltaTime);
                update(deltaTime);
                onAppPostUpdate(deltaTime);

                // HACK: To solve Segfault in IMGUI, have to check it's running here again, because after we switch state
                // in update() method, it calls exit, but it still continues through the loop and hits render function and that causes
                // imgui to try and render on empty context
                if (isRunning) render(deltaTime);

                //std::cout << deltaTime << std::endl;
                //std::cout <<  " T: " << totalDeltaTime << std::endl;
                totalDeltaTime -= deltaTime;

                //if we have reached the maximum physics steps, just continue on with the frame
                updateCount++;
            }

            fps = frameLimiter.end();
            // TODO: window title is currently isolated in init() method, we can drop that and load
            // it from engine class instead using LuaScript?
            // BUFFER OVERFLOW ON LINUX, YAY
            char titleBuffer[32];
            //sprintf(titleBuffer, "Villain Engine Test. FPS: %f.2", fps);
            window.setWindowTitle(titleBuffer);

            //swap window buffer for less flickering
            window.swapBuffer();
        }
    }

    /**
     * Renders current state
     * @param deltaTime
     */
    void Engine::render(float deltaTime){

        glEnable(GL_DEPTH_TEST);
        //enable alpha blending
        glEnable(GL_BLEND);
        //what kind of blending we want
        //in this case, we want alpha 0 to be transparent
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // These commands probably should go just before update() method so that states can setup their own ui
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // In debug/edit mode render scene to texture and output it in imgui
        if (debugMode)
            sceneBuffer->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearDepth(1.0);
        //SDL_GetWindowSize(window.getSDLWindow(), &screenWidth, &screenHeight);
        //glViewport(0, 0, screenWidth, screenHeight);

        // First render application
        if (currentState && currentState->getScreenState() == ScreenState::RUNNING) {
            currentState->draw(deltaTime);
        }
        onAppRender(deltaTime);

        if (debugMode)
            sceneBuffer->unbind();

        // Then render Nuklear UI
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        // In the end Render ImGui
        if (debugMode) {
            SDL_ShowCursor(SDL_TRUE);
            DebugConsole::Instance()->render();

            if (currentState && currentState->getScreenState() == ScreenState::RUNNING) {
                currentState->onImGuiDraw(deltaTime);
            }
            onAppImGuiRender(deltaTime);
        }

        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
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

    void Engine::onAppImGuiRender(float deltaTime) {
        ImGui::Begin("Scene");
        {
            ImGui::BeginChild("GameRender");

            float width = ImGui::GetContentRegionAvail().x;
            float height = ImGui::GetContentRegionAvail().y;

            ImGui::Image(
                    (ImTextureID)sceneBuffer->getTextureID(),
                    //ImGui::GetContentRegionAvail(),
                    ImGui::GetWindowSize(),
                    ImVec2(0, 1),
                    ImVec2(1, 0)
                    );
            ImGui::EndChild();
        }
        ImGui::End();


        ImGui::Begin("Asset Browser");
        {
            if (ImGui::TreeNode("Assets")) {
                if (ImGui::TreeNode("Shaders")) {
                    for (auto const& t: ResourceManager::Instance()->getShaderMap()) {
                        ImGui::TreeNode(t.first.c_str());
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Textures")) {
                    for (auto const& t: ResourceManager::Instance()->getTextureMap()) {
                        ImGui::TreeNode(t.first.c_str());
                    }

                    ImGui::TreePop();
                }
                // TODO: Sounds, fonts, levels

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    /**
     * Update current state or change state if needed
     * @param deltaTime
     * @sa StateMachine
     */
    void Engine::update(float deltaTime) {
        // update() method is used in engine render because it also renders particles
        //ParticleSystem::Instance()->update(deltaTime);

        /*for (auto& e: EntityManager::Instance()->getEntities()) {
            // Find player and update camera
            if (e->hasComponent<InputComponent>()) {
                TheEngine::Instance()->camera.x = e->transform->getX() - TheEngine::Instance()->camera.w/2;
                TheEngine::Instance()->camera.y = e->transform->getY() - TheEngine::Instance()->camera.h/2;
                break;
            }
        }*/

        if (currentState) {
            switch (currentState->getScreenState()) {
                //update the current running screen
                case ScreenState::RUNNING:
                    currentState->update(deltaTime);
                    break;
                    //change to next screen
                case ScreenState::CHANGE_NEXT:
                    //clean up running screen
                    currentState->onExit();
                    currentState = stateMachine->moveNext();
                    if (currentState) {
                        //initialize new running screen
                        currentState->setRunning();
                        currentState->onEntry();
                    }
                    break;
                    //change to previous screen
                case ScreenState::CHANGE_PREVIOUS:
                    //clean up running screen
                    currentState->onExit();
                    currentState = stateMachine->movePrev();
                    if (currentState) {
                        //initialize new running screen
                        currentState->setRunning();
                        currentState->onEntry();
                    }
                    break;
                    //exit game
                case ScreenState::EXIT_APPLICATION:
                    exit();
                    break;
                default:
                    break;
            }
        }
        //else { exit(); }
    }

    /**
     * Takes SDL_Event reference from current running state and according to event types to InputManager.
     * @param event SDL_Event& structure
     * @sa GameState::update, InputManager
     */
    void Engine::handleEvents(SDL_Event& event) {
        static bool mouseFirst = true;
        //set the event type
        TheInputManager::Instance()->setEventType(event.type);
        switch (event.type) {
            case SDL_QUIT:
                exit();
                break;
            case SDL_MOUSEMOTION:
                mouseMotion = true;
                // Set offsets if relative mouse mode is used for 3D camera
                if (!mouseFirst) {
                    TheInputManager::Instance()->setMouseOffsets((float)event.motion.xrel, (float)event.motion.yrel);
                } else {
                    mouseFirst = false;
                    TheInputManager::Instance()->setMouseOffsets(0.0f, 0.0f);
                }

                TheInputManager::Instance()->setMouseCoords((float)event.motion.x, (float)event.motion.y);
                onMouseMove(event.motion.x, event.motion.y);
                break;
            case SDL_KEYDOWN:
                TheInputManager::Instance()->pressKey(event.key.keysym.sym);
                // Find player and send key code to Lua listener
                // Really don't like how you have to loop through all entities to find player, on the other hand player will
                // be one of the first entities defined in lua
                //for (auto& e: EntityManager::Instance()->getEntities()) {
                    //if (e->hasComponent<InputComponent>()) {
                        //ScriptEngine::Instance()->dispatch(e->getListener("INPUT"), e->id->get(), event.key.keysym.sym);
                        //break;
                    //}
                //}
                if (event.key.keysym.sym == SDLK_BACKQUOTE)
                    debugMode = !debugMode;
                break;
            case SDL_KEYUP:
                TheInputManager::Instance()->releaseKey(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                TheInputManager::Instance()->pressKey(event.button.button);
                onMouseDown(event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                TheInputManager::Instance()->releaseKey(event.button.button);
                onMouseUp();
                break;
            case SDL_TEXTINPUT:
                TheInputManager::Instance()->addInputCharacters(event.text.text);
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0)
                    TheInputManager::Instance()->setMouseWheel(1);
                if (event.wheel.y < 0)
                    TheInputManager::Instance()->setMouseWheel(-1);
                break;
            case (SDLK_ESCAPE):
                isRunning = false;
                break;
            default:
                break;
        }
        switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                screenWidth = event.window.data1;
                screenHeight = event.window.data2;
                glViewport(0, 0, screenWidth, screenHeight);
                sceneBuffer->rescale(screenWidth, screenHeight);
                onAppWindowResize(screenWidth, screenHeight);
                break;
            default:
                break;
        }
    }

    void Engine::exit() {

        if (currentState != nullptr) {
            currentState->onExit();
        }

        if (stateMachine) {
            stateMachine->destroy();
            stateMachine.reset();
        }

        isRunning = false;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        TTF_Quit();
        //IMG_Quit();

        nk_sdl_shutdown();
        SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
        SDL_DestroyWindow(window.getSDLWindow());
        SDL_Quit();
    }
}
