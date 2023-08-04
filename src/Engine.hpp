#pragma once

#include "EventDispatcher.hpp"
#include "Input.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#include "audio/AudioEngine.hpp"
#include "editor/DebugConsole.hpp"
#include "editor/ImGuiLayer.hpp"
#include "physics/ParticleWorld.hpp"
#include "physics/RigidBodyWorld.hpp"
#include "rendering/FrameBuffer.hpp"
#include "rendering/RendereringEngine.hpp"
#include "ui/UILayer.hpp"

#include <functional>
#include <stdio.h>
#include <memory>
#include <vector>

namespace Villain {

    class Application;
    class Entity;

    /*! \brief Main engine class containing game loop.
     *
     * Responsible for managing game loop with semi-fixed time step. Initialises most of the engine's subsystems,
     * including but not limited to Window, RenderingEngine, RigidBodyWorld, EventDispatcher and client Application.
     * Also handles input events and allows using Lua config script to setup startup settings.
     */
    class Engine {
        public:
            Engine();
            virtual ~Engine();

            /// Initialise engine using explicit params.
            void init(Application* app, std::string title, int height, int width, unsigned int windowFlags, bool enableGammaCorrection = false);
            /// Initialise engine using Lua config script
            void init(Application* app, const std::string& luaConfigPath);
            /// Start running main application/game loop with semi fixed time step
            void run();
            /// Cleanup resources and exit engine application
            void exit();
            /// Handle input events and dispatch them using EventDispatcher and update InputManager
            void handleEvents(SDL_Event& event);

            bool running(){ return isRunning; }
            static void setRunning(bool running) { isRunning = running; }
            static void setEditMode(bool edit) { editMode = edit; }

            static float getFps() { return fps; }
            static uint32_t getUpdateTime() { return updateTime; }
            static uint32_t getRenderTime() { return renderTime; }
            static bool editModeActive() { return editMode; }
            bool* wireFrameModeActive() { return &wireFrameMode; }

            /// Special one-time render pass used for displaying loading screen image while the user application is loading
            void renderLoadingScreen();

            inline ParticleWorld* getParticleWorld() { return particleWorld.get(); }
            inline RigidBodyWorld* getRigidBodyWorld() { return rigidBodyWorld.get(); }
            inline RenderingEngine* getRenderingEngine() { return renderingEngine.get(); }
            inline Application* getApplication() { return application; }
            inline EventDispatcher* getEventDispatcher() { return eventDispatcher.get(); }

            FrameBuffer* getSceneBuffer() { return sceneBuffer.get(); }
            ImGuiLayer& getImGuiLayer() { return imGuiLayer; }
            UILayer& getUILayer() { return nuklearLayer; }
            UIManager& getUIManager() { return nuklearLayer.getManager(); }
            Window getWindow() { return window; }

            static int getScreenWidth();
            static int getScreenHeight();
        private:
            void render(float deltaTime); ///< Main render method, renders current state

            static bool isRunning; ///< appliction running flag
            static bool editMode; ///< enables IMGui Overlay
            Window window; ///< main window instance
            ImGuiLayer imGuiLayer; ///< Responsible for initialising and rendering ImGui ui
            UILayer nuklearLayer; ///< Responsible for initialising and rendering nuklear ui
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
            std::unique_ptr<ParticleWorld> particleWorld = nullptr;
            std::unique_ptr<RigidBodyWorld> rigidBodyWorld = nullptr;
            std::unique_ptr<RenderingEngine> renderingEngine = nullptr;
            Application* application = nullptr; ///< User engine application
            std::unique_ptr<EventDispatcher> eventDispatcher;
            AudioEngine audioEngine;
    };
}
