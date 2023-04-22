#include "Engine.hpp"

#include "Application.hpp"
#include "FrameLimiter.hpp"
#include "ErrorHandler.hpp"

#include <string>
#include <cstdio> // For sprintf
#include "Logger.hpp"

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
    bool Engine::isRunning = false;
    bool Engine::editMode = false;
    float Engine::fps = 0;

    int Engine::getScreenWidth() { return screenWidth; }
    int Engine::getScreenHeight() { return screenHeight; }

    Engine::Engine() {
        Logger::Instance()->info("Initialising the engine.");
    }

    Engine::~Engine() {}

    void Engine::init(Application* app, std::string title, int height, int width, unsigned int windowFlags) {
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

        window.create(title, screenHeight, screenWidth, windowFlags);

        imGuiLayer.init(window);

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

        // TODO: will need some configs here, gravity vector for example
        physicsEngine = std::make_unique<PhysicsEngine>();
        // NOTE: must be initialized before application
        renderingEngine = new RenderingEngine(this);

        //initialize the current game
        application = app;
        application->setEngine(this);
        application->setNulkearContext(nuklearContext);
        application->init();

        //initialize game screens and add them to the screenList
        application->addStates();

        //set the MainGame's current game screen
        application->startStateMachine();


        isRunning = true;//start main loop
    }

    void Engine::init(Application* app, const std::string& luaConfigPath) {
        LuaScript configScript(luaConfigPath);
        configScript.open();

        unsigned int flags = 0;
        if (configScript.get<bool>("window.fullscreen"))
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        if (configScript.get<bool>("window.borderless"))
            flags |= SDL_WINDOW_BORDERLESS;
        if (configScript.get<bool>("window.resizable"))
            flags |= SDL_WINDOW_RESIZABLE;

        init(
            app,
            configScript.get<std::string>("window.title"),
            configScript.get<int>("window.height"),
            configScript.get<int>("window.width"),
            flags
        );
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

                application->handleEvents(deltaTime);

                physicsEngine->simulate(deltaTime);
                physicsEngine->handleCollisions();

                application->onAppPreUpdate(deltaTime);
                application->update(deltaTime);
                application->onAppPostUpdate(deltaTime);

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

        imGuiLayer.start();

        // In debug/edit mode render scene to texture and output it in imgui
        if (editMode)
            sceneBuffer->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearDepth(1.0);
        //SDL_GetWindowSize(window.getSDLWindow(), &screenWidth, &screenHeight);
        //glViewport(0, 0, screenWidth, screenHeight);

        // First render application
        // NOTE: we want to have only 1 render method here in the end preferably and just
        // let rendering engine take care of all things?
        application->render(renderingEngine);
        application->onAppRender(deltaTime);

        if (editMode)
            sceneBuffer->unbind();

        // Then render Nuklear UI
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        // In the end Render ImGui
        if (editMode) {
            // Clear background
            glClear(GL_COLOR_BUFFER_BIT);
            SDL_ShowCursor(SDL_TRUE);
            SDL_SetRelativeMouseMode(SDL_FALSE);
            imGuiLayer.render(*this);

            //if (currentState && currentState->getScreenState() == ScreenState::RUNNING) {
                //currentState->onImGuiDraw(deltaTime);
            //}
            application->onAppImGuiRender(deltaTime);
        }

        imGuiLayer.end();
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
                application->onMouseMove(event.motion.x, event.motion.y);
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
                    editMode = !editMode;
                break;
            case SDL_KEYUP:
                TheInputManager::Instance()->releaseKey(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                TheInputManager::Instance()->pressKey(event.button.button);
                application->onMouseDown(event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                TheInputManager::Instance()->releaseKey(event.button.button);
                application->onMouseUp();
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
                application->onAppWindowResize(screenWidth, screenHeight);
                renderingEngine->resizeCameras(screenWidth, screenHeight);
                break;
            default:
                break;
        }
    }

    void Engine::exit() {
        isRunning = false;

        imGuiLayer.exit();

        TTF_Quit();
        //IMG_Quit();

        nk_sdl_shutdown();
        SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
        SDL_DestroyWindow(window.getSDLWindow());
        SDL_Quit();
    }
}
