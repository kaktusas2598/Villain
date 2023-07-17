#include "Engine.hpp"

#include "Application.hpp"
#include "FrameLimiter.hpp"
#include "ErrorHandler.hpp"

// Events
#include "events/KeyboardEvent.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowResizeEvent.hpp"

#include <string>
#include <cstdio> // For sprintf
#include "Logger.hpp"

#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#include "glm/gtc/matrix_transform.hpp"

// For loading screen
#include "rendering/Mesh.hpp"
#include "FileUtils.hpp"

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
    uint32_t Engine::updateTime = 0;
    uint32_t Engine::renderTime = 0;

    int Engine::getScreenWidth() { return screenWidth; }
    int Engine::getScreenHeight() { return screenHeight; }

    Engine::Engine() {
        Logger::Instance()->info("Initialising the engine.");
    }

    Engine::~Engine() {}

    void Engine::init(Application* app, std::string title, int height, int width, unsigned int windowFlags, bool enableGammaCorrection) {
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
            //struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
            nk_sdl_font_stash_end();
            //nk_style_load_all_cursors(nuklearContext, atlas->cursors);
            //nk_style_set_font(nuklearContext, &roboto->handle);
        }

        sceneBuffer = std::make_unique<FrameBuffer>(screenWidth, screenHeight);

        // TODO: will need some configs here, gravity vector for example
        physicsEngine = std::make_unique<PhysicsEngine>(this);
        // TODO: ability to set custom number of contacts
        particleWorld = std::make_unique<ParticleWorld>(200);
        rigidBodyWorld = std::make_unique<RigidBodyWorld>();
        // NOTE: must be initialized before application
        renderingEngine = std::make_unique<RenderingEngine>(this);

        eventDispatcher = std::make_unique<EventDispatcher>();

        if (enableGammaCorrection)
            renderingEngine->setGammaCorrection(true);

        renderLoadingScreen();

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
            flags,
            configScript.get<bool>("rendering.gammaCorrection")
        );
    }

    // Special one-time render pass, displayed while user application is loading
    void Engine::renderLoadingScreen() {
        // TODO: Handle Resize, embed loading screen texture
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // Setting up screen quad
        std::vector<VertexP1C1UV> vertices;
        vertices.push_back({{-1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0}});
        vertices.push_back({{-1.0, 1.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 1.0}});
        vertices.push_back({{1.0, 1.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0}});
        vertices.push_back({{1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 0.0}});
        std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
        Mesh<VertexP1C1UV> quad(vertices, indices);

        Shader* shader = Shader::createFromResource("spriteBatch");

        cmrc::file resourceFile = FileUtils::loadResource("res/textures/logo.jpeg");
        Texture* loadingLogo = new Texture(resourceFile.size(), const_cast<char*>(&(*resourceFile.begin())), false);
        Material material{"loadingScreen", loadingLogo, 1};

        loadingLogo->bind();
        shader->bind();
        shader->setUniform1i("spriteTexture", 0);
        shader->setUniformMat4f("model", glm::mat4(1.0f));
        shader->setUniformMat4f("view", glm::mat4(1.0f));
        shader->setUniformMat4f("projection", glm::mat4(1.0f));
        // TODO: there should be no need to set material as well, for sprite we can only use diffuse texture and avoid non-existant uniform warnings
        quad.draw(*shader, material);

        window.swapBuffer();
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

            int updateCount = 0;
            float deltaTime = 0;

            nk_input_begin(nuklearContext);

            profiler.start();
            // Semi-Fixed time step with number of steps to prevent "Spiral of Death" if update logic takes too long,
            // but it might cause a bit of a slowdown for physics systems under heavy load
            // NOTE: Investigate fixed-time step with interpolation instead for
            // super precise simulations, read: https://gafferongames.com/post/fix_your_timestep/
            while (totalDeltaTime > 0.0f && updateCount < MAX_PHYSICS_STEPS && isRunning) {
                // NOTE: Probably not correct to have input handling in here, but if we move this outside semi-fixed step
                // loop, events are not being caught in example application's update() methods
                //update input manager
                Input::Get()->update();
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    ImGui_ImplSDL2_ProcessEvent(&event);
                    handleEvents(event);
                    nk_sdl_handle_event(&event);
                }

                //limit deltatime to 1.0 so no speedup (1.0 being one frame and .2 being a fifth of a frame)
                deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);

                deltaTime = deltaTime / DESIRED_FPS;

                rigidBodyWorld->startFrame();

                application->handleEvents(deltaTime);

                physicsEngine->simulate(deltaTime);
                physicsEngine->handleCollisions();

                particleWorld->runPhysics(deltaTime);


                application->onAppPreUpdate(deltaTime);
                application->update(deltaTime);
                application->onAppPostUpdate(deltaTime);

                rigidBodyWorld->runPhysics(deltaTime);

                totalDeltaTime -= deltaTime;

                //if we have reached the maximum physics steps, just continue on with the frame
                updateCount++;
            }
            updateTime = profiler.read();

            render(deltaTime);

            renderTime = profiler.read();

            nk_input_end(nuklearContext);

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

        if (wireFrameMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // First render application
        // NOTE: we want to have only 1 render method here in the end preferably and just
        // let rendering engine take care of all things?
        application->render(renderingEngine.get(), deltaTime);

        application->onAppRender(deltaTime);
        physicsEngine->render();

        // Make sure we disable wireframe mode before post processing pass
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Render additional fbos, apply post-processing fx
        application->postRenderPass(renderingEngine.get());

        // If in editor mode make sure to unbind fbos, so that we can render scene buffer in editor's viewport
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
    // TODO: probably gradually we will want to phase out InputManager singleton with new Event system
    void Engine::handleEvents(SDL_Event& event) {
        static bool mouseFirst = true;
        //set the event type
        Input::Get()->setEventType(event.type);
        switch (event.type) {
            case SDL_QUIT:
                exit();
                break;
            case SDL_MOUSEMOTION:
                mouseMotion = true;
                {
                    glm::vec2 offsets{0.0f};
                    // Set offsets if relative mouse mode is used for 3D camera
                    if (!mouseFirst) {
                        offsets = {event.motion.xrel, event.motion.yrel};
                    } else {
                        mouseFirst = false;
                    }
                    Input::Get()->setMouseOffsets(offsets.x, offsets.y);

                    MouseEvent mouseEvent = MouseEvent(MouseEventType::MOVE, {event.motion.x, event.motion.y}, offsets);
                    eventDispatcher->dispatchEvent(mouseEvent);
                }

                Input::Get()->setMouseCoords((float)event.motion.x, (float)event.motion.y);
                application->onMouseMove(event.motion.x, event.motion.y);
                break;
            case SDL_KEYDOWN:
                {
                    KeyboardEvent keyboardEvent = KeyboardEvent(KeyEventType::PRESS, static_cast<KeyCode>(event.key.keysym.sym));
                    eventDispatcher->dispatchEvent(keyboardEvent);
                }
                Input::Get()->pressKey(event.key.keysym.sym);
                if (event.key.keysym.sym == SDLK_BACKQUOTE)
                    editMode = !editMode;
                break;
            case SDL_KEYUP:
                {
                    KeyboardEvent keyboardEvent = KeyboardEvent(KeyEventType::RELEASE, static_cast<KeyCode>(event.key.keysym.sym));
                    eventDispatcher->dispatchEvent(keyboardEvent);
                }
                Input::Get()->releaseKey(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    MouseEvent mouseEvent = MouseEvent(MouseEventType::CLICK, event.button.button);
                    eventDispatcher->dispatchEvent(mouseEvent);
                }
                Input::Get()->pressKey(event.button.button);
                application->onMouseDown(event.button.x, event.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                {
                    MouseEvent mouseEvent = MouseEvent(MouseEventType::RELEASE, event.button.button);
                    eventDispatcher->dispatchEvent(mouseEvent);
                }
                Input::Get()->releaseKey(event.button.button);
                application->onMouseUp();
                break;
            case SDL_TEXTINPUT:
                Input::Get()->addInputCharacters(event.text.text);
                break;
            case SDL_MOUSEWHEEL:
                {
                    MouseEvent mouseEvent = MouseEvent(MouseEventType::SCROLL, event.wheel.y);
                    eventDispatcher->dispatchEvent(mouseEvent);
                }
                if (event.wheel.y > 0)
                    Input::Get()->setMouseWheel(1);
                if (event.wheel.y < 0)
                    Input::Get()->setMouseWheel(-1);
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
                sceneBuffer->rescale(screenWidth, screenHeight);
                //ImGuiIO& io = ImGui::GetIO();
                //io.DisplaySize = ImVec2(screenWidth, screenHeight);
                //io.DisplayFramebufferScale = ImVec2(1.0, 1.0);
                glViewport(0, 0, screenWidth, screenHeight);
                {
                    WindowResizeEvent resizeEvent = WindowResizeEvent(screenWidth, screenHeight);
                    eventDispatcher->dispatchEvent(resizeEvent);
                }
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
