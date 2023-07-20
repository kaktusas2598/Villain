#pragma once

#include "EventDispatcher.hpp"
#include "Input.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#include "editor/DebugConsole.hpp"
#include "editor/ImGuiLayer.hpp"
#include "physics/ParticleWorld.hpp"
#include "physics/PhysicsEngine.hpp"
#include "physics/RigidBodyWorld.hpp"
#include "rendering/FrameBuffer.hpp"
#include "rendering/RendereringEngine.hpp"

#include <functional>
#include <stdio.h>
#include <memory>
#include <vector>

struct nk_context;

namespace Villain {

    class Application;
    class Entity;

    /*!
     * Main game loop class responsible for setting Window, Rendering Context and main engine subsystems
     */
    class Engine {
        public:
            Engine();
            virtual ~Engine();

            /**
             * init systems, create window and GL context
             * @param title Title to be displayed on window title bar
             * @param height Screen height in pixels
             * @param width Screen width in pixels
             * @param windowFlags window flags
             * @sa WindowFlags
             */
            void init(Application* app, std::string title, int height, int width, unsigned int windowFlags, bool enableGammaCorrection = false);
            /// Initialise engine using Lua config script
            void init(Application* app, const std::string& luaConfigPath);
            /// Main application/game loop with semi fixed time step
            void run();
            /// Cleanup resources and exit engine application
            void exit();

            void handleEvents(SDL_Event& event);

            bool running(){ return isRunning; }
            static void setRunning(bool running) { isRunning = running; }

            static float getFps() { return fps; }
            static uint32_t getUpdateTime() { return updateTime; }
            static uint32_t getRenderTime() { return renderTime; }
            static bool editModeActive() { return editMode; }
            bool* wireFrameModeActive() { return &wireFrameMode; }
            void renderLoadingScreen();

            inline PhysicsEngine* getPhysicsEngine() { return physicsEngine.get(); }
            inline ParticleWorld* getParticleWorld() { return particleWorld.get(); }
            inline RigidBodyWorld* getRigidBodyWorld() { return rigidBodyWorld.get(); }
            inline RenderingEngine* getRenderingEngine() { return renderingEngine.get(); }
            inline Application* getApplication() { return application; }
            inline EventDispatcher* getEventDispatcher() { return eventDispatcher.get(); }

            FrameBuffer* getSceneBuffer() { return sceneBuffer.get(); }
            ImGuiLayer& getImGuiLayer() { return imGuiLayer; }
            Window getWindow() { return window; }

            static int getScreenWidth();
            static int getScreenHeight();
        private:
            void render(float deltaTime); ///< Main render method, renders current state

            static bool isRunning; ///< appliction running flag
            static bool editMode; ///< enables IMGui Overlay
            Window window; ///< main window instance
            ImGuiLayer imGuiLayer; ///< Responsible for initialising and rendering ImGui ui
            bool wireFrameMode = false; ///< Optional wireframe rendering mode

            // Screen dimensions will be static so they can be accessed from anywhere in the Engine
            static int screenWidth;
            static int screenHeight;

            static float fps; ///< main application's fps
            static uint32_t updateTime;
            static uint32_t renderTime;
            Timer profiler;
            bool mouseMotion = false;

            std::unique_ptr<FrameBuffer> sceneBuffer = nullptr; ///< Render Application scene here for futher processing
            struct nk_context* nuklearContext; ///< Nuklear UI context

            std::unique_ptr<PhysicsEngine> physicsEngine = nullptr;
            std::unique_ptr<ParticleWorld> particleWorld = nullptr;
            std::unique_ptr<RigidBodyWorld> rigidBodyWorld = nullptr;
            std::unique_ptr<RenderingEngine> renderingEngine = nullptr;
            Application* application = nullptr; ///< User engine application
            std::unique_ptr<EventDispatcher> eventDispatcher;
    };
}
