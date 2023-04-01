#ifndef __Engine__
#define __Engine__

#include "DebugConsole.hpp"
#include "FrameBuffer.hpp"
#include "ImGuiLayer.hpp"
#include "InputManager.hpp"
#include "RendereringEngine.hpp"
#include "Window.hpp"

#include <functional>
#include <stdio.h>
#include <memory>
#include <vector>

struct nk_context;

namespace Villain {

    class Application;
    class Entity;

    /*! \brief Engine
     * Main Application Class, entrypoint class will need to extend this one
     *
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
            void init(Application* app, std::string title, int height, int width, unsigned int windowFlags);
            // Or initialise using Lua config script
            void init(Application* app, const std::string& luaConfigPath);
            void run(); ///< runs main application's loop
            void exit(); //< clean resources and exit application

            void handleEvents(SDL_Event& event);

            bool running(){ return isRunning; }
            static void setRunning(bool running) { isRunning = running; }

            static float getFps() { return fps; }
            static bool editModeActive() { return editMode; }

            inline RenderingEngine* getRenderingEngine() { return renderingEngine; }
            inline Application* getApplication() { return application; }
            FrameBuffer* getSceneBuffer() { return sceneBuffer.get(); }
            Window getWindow() { return window; }

            static int getScreenWidth();
            static int getScreenHeight();
        private:
            void render(float deltaTime); ///< Main render method, renders current state

            static bool isRunning; ///< appliction running flag
            static bool editMode; ///< enables IMGui Overlay
            Window window; ///< main window instance
            ImGuiLayer imGuiLayer; ///< Responsible for initialising and rendering ImGui ui

            // Screen dimensions will be static so they can be accessed from anywhere in the Engine
            static int screenWidth;
            static int screenHeight;

            static float fps; ///< main application's fps
            bool mouseMotion = false;

            std::unique_ptr<FrameBuffer> sceneBuffer = nullptr;
            struct nk_context* nuklearContext;

            RenderingEngine* renderingEngine = nullptr;
            Application* application = nullptr;
    };
}

#endif // __Engine__
