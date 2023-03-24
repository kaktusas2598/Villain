#ifndef __Engine__
#define __Engine__

#include "FrameBuffer.hpp"
#include "Window.hpp"
#include "InputManager.hpp"
#include "DebugConsole.hpp"

#include "StateMachine.hpp"
//#include "Level.hpp"

#include <functional>
#include <stdio.h>
#include <memory>
#include <vector>

struct nk_context;

namespace Villain {

    class IGameScreen;
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
             * @param currentFlags window flags
             * @param sdlEnabled Optional SDL rendering
             * @sa WindowFlags
             */
            void init(std::string title, int height, int width, unsigned int currentFlags, bool sdlRendering = false);
            void run(); ///< runs main application's loop
            void exit(); //< clean resources and exit application

            virtual void onInit() = 0;
            virtual void addStates();
            virtual void onExit();

            void handleEvents(SDL_Event& event);

            bool running(){ return isRunning; }
            void setRunning(bool running) { isRunning = running; }

            const float getFps() const { return fps; }

            StateMachine* getStateMachine() { return stateMachine.get(); }
            Window getWindow() { return window; }
            struct nk_context * getNuklearContext() { return nuklearContext; }

            static int getScreenWidth();
            static int getScreenHeight();

            // Mainly used for Lua to get current level dimensions
            //int getMapWidth() const { return level != nullptr ? level->getWidth() : 0; }
            //int getMapHeight() const { return level != nullptr ? level->getHeight() : 0; }
            // Current level or nullptr if none
            //void setLevel(Level* lvl) { level = lvl; }
            //Level* getLevel() { return level; }

            // Any application using engine must implement these callbacks
            virtual void onAppPreUpdate(float deltaTime) = 0;
            virtual void onAppPostUpdate(float deltaTime) = 0;
            virtual void onAppRender(float deltaTime) = 0;
            virtual void onAppWindowResize(int newWidth, int newHeight) = 0;
            virtual void onAppImGuiRender(float deltaTime) {}

            // Optional callbacks
            virtual void onMouseMove(int mouseX, int mouseY) {}
            virtual void onMouseDown(int mouseX, int mouseY) {}
            virtual void onMouseUp() {}

        protected:
            void render(float deltaTime); ///< Main render method, renders current state
            void update(float deltaTime); ///< Main update method, sets different state or updates current one

            bool isRunning = false; ///< appliction running flag
            bool debugMode = false; ///< enables IMGui Debug Console
            Window window; ///< main window instance

            // Screen dimensions will be static so they can be accessed from anywhere in the Engine
            static int screenWidth;
            static int screenHeight;

            float fps = 0; ///< main application's fps

            std::unique_ptr<StateMachine> stateMachine = nullptr; ///< state machine's instance
            //GameState* currentState = nullptr; ///< current state's instance
            IGameScreen* currentState = nullptr; ///< current state's instance

            //std::vector<Entity*> entities;
            //Level* level;
        private:
            struct nk_context* nuklearContext;
    };
}

#endif // __Engine__
