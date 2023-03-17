#include "Engine.hpp"
#include "FrameLimiter.hpp"
#include "ErrorHandler.hpp"
//#include "EntityManager.hpp"

//#include "StateParser.hpp"
//#include "GameState.hpp"

#include <string>
#include <cstdio> // For sprintf
#include "Logger.hpp"

//#include "ParticleSystem.hpp"
//#include "Collision.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/gtc/matrix_transform.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace Villain {

    int Engine::screenWidth;
    int Engine::screenHeight;

    int Engine::getScreenWidth() { return screenWidth; }
    int Engine::getScreenHeight() { return screenHeight; }

    Engine::Engine() {
        // Initialize State Machine
        Logger::Instance()->info("Initialising the engine.");
        //stateMachine = std::make_unique<StateMachine>(this);
        // Set default level
        //level = nullptr;
    }

    Engine::~Engine() {}

    void Engine::addStates() {
        //Logger::Instance()->info("Adding states.");
        //StateParser stateParser;
        //std::vector<GameState*> states;
        //if (!stateParser.loadStates("state.xml", &states)) {
            //Logger::Instance()->error("Failed adding states.");
        //}
        //for (auto& state: states) {
            //stateMachine->addState(state);
        //}
        //stateMachine->setState(states[0]->getID());
        //states.clear();

        return;
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
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);//?

        window.create(title, screenHeight, screenWidth, currentFlags);

        //initialize the current game
        // onInit();
        //Initialise game camera
        //camera = {0, 0, screenWidth, screenHeight};

        //initialize game screens and add them to the screenList
        addStates();

        //set the MainGame's current game screen
        //currentState = stateMachine->getCurrentState();

        //initialize game screen elements
        //currentState->onEntry();
        //set the initial game screen to ScreenState::RUNNING
        //currentState->setRunning();

        glEnable(GL_DEPTH_TEST);

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

            //update input manager
            TheInputManager::Instance()->update();

            //SEMI FIXED TIME STEP ??
            int updateCount = 0;
            while (totalDeltaTime > 0.0f && updateCount < MAX_PHYSICS_STEPS && isRunning) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    ImGui_ImplSDL2_ProcessEvent(&event);
                    handleEvents(event);
                }

                //limit deltatime to 1.0 so no speedup (1.0 being one frame and .2 being a fifth of a frame)
                float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);

                deltaTime = deltaTime / DESIRED_FPS;

                onAppPreUpdate(deltaTime);
                update(deltaTime);
                onAppPostUpdate(deltaTime);

                render(deltaTime);
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

        // These commands probably should go just before update() method so that states can setup their own ui
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (debugMode) {
            DebugConsole::Instance()->render();
        }

        // ImGui::Render(); in imgui example Render() was placed here before SDL_RenderClear
        //SDL_SetRenderDrawColor(window.getSDLRenderer(), (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));

        //SDL_RenderClear(window.getSDLRenderer()); // clear the renderer to the draw color
        //if (currentState && currentState->getScreenState() == ScreenState::RUNNING) {
        //currentState->draw(deltaTime);
        //}
        // TEMPORARY, just testing EntityManager, entities will get rendered twice
        //EntityManager::Instance()->render(deltaTime);

        //ParticleSystem::Instance()->update(deltaTime); // Update actually does rendering too
        //ParticleSystem::Instance()->render(deltaTime);

        //glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        onAppRender(deltaTime);

        ImGui::Render();
        //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //{
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        //}

        ImGui::EndFrame();
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
        }

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
                        //initialize zanew running screen
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
        else { exit(); }

        EntityManager::Instance()->refresh();
        EntityManager::Instance()->update(deltaTime);

        for (auto& e: EntityManager::Instance()->getEntities()) {
            // Chek each entity for collision against map tiles
            Collision::checkMapCollision(e, level->getCollidableLayers());

            if (e->hasComponent<InputComponent>()) {
                // FPS DROP from 60 to 20 and below, caused by particle emitters in onCollide() listener in Lua
                // Removing that and looks like this method does not cause massive lag anymore
                Collision::checkPlayerEntityCollision(e, EntityManager::Instance()->getEntities());
            } else if (e->hasComponent<ProjectileComponent>()) {
                Collision::checkProjectileEntityCollision(e, EntityManager::Instance()->getEntities());
            }
        }*/
    }

    /**
     * Takes SDL_Event reference from current running state and according to event types to InputManager.
     * @param event SDL_Event& structure
     * @sa GameState::update, InputManager
     */
    void Engine::handleEvents(SDL_Event& event) {
        //set the event type
        TheInputManager::Instance()->setEventType(event.type);
        switch (event.type) {
            case SDL_QUIT:
                exit();
                break;
            case SDL_MOUSEMOTION:
                TheInputManager::Instance()->setMouseCoords((float)event.motion.x, (float)event.motion.y);
                onMouseMove(event.motion.x, event.motion.y);

                // Just a testing code for particle emitters
                //ParticleSystem::Instance()->addEmitter(Vector2D{(float)event.motion.x, (float)event.motion.y}, "fire");
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
                onMouseDown(event.motion.x, event.motion.y);
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
                onAppWindowResize(screenWidth, screenHeight);
                // TODO: Update camera viewports
                //screenWidth = camera.w = event.window.data1;
                //screenHeight = camera.h = event.window.data2;
                // Update map
                /*if (level != nullptr) {
                    for (auto it = level->getLayers()->begin(); it != level->getLayers()->end(); ++it) {
                        (*it)->init();
                    }
                }
                // Update UI
                for (auto& e: EntityManager::Instance()->getEntities()) {
                    // Need much better way than this, probably using render groups? but every component belonging to group
                    // needs to have same method, extend from another class?
                    if (e->hasComponent<UILabelComponent>()) {
                        e->getComponent<UILabelComponent>()->ui.refresh();
                        continue;
                    } else if (e->hasComponent<ButtonComponent>()) {
                        e->getComponent<ButtonComponent>()->ui.refresh();
                        continue;
                    } else if (e->hasComponent<BackgroundComponent>()) {
                        e->getComponent<BackgroundComponent>()->reload();
                        continue;
                    }
                }

                SDL_RenderPresent( window.getSDLRenderer() );*/
                break;
            default:
                break;
        }
    }

    void Engine::exit() {

        /*currentState->onExit();

        if (stateMachine)
        {
            stateMachine->destroy();
            stateMachine.reset();
        }

        ParticleSystem::Instance()->destroy();
        ParticleSystem::Instance()->clean();*/


        isRunning = false;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        TTF_Quit();
        //IMG_Quit();

        //SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window.getSDLWindow());
        SDL_Quit();
    }
}
