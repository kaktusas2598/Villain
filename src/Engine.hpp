#ifndef __Engine__
#define __Engine__

#include "Window.hpp"
#include "InputManager.hpp"
#include "DebugConsole.hpp"
//#include "StateMachine.hpp"
//#include "Level.hpp"

#include <stdio.h>
#include <memory>
#include <vector>

namespace Villain {

    class GameState;
    class Entity;

    /*! \brief Engine
     *         Main Engine Class.
     *
     */
    class Engine {
        public:
            static Engine* Instance() {
                if (s_pInstance == 0) {
                    s_pInstance = new Engine();
                    return s_pInstance;
                }
                return s_pInstance;
            }
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

            // void onInit();
            void addStates();
            void onExit();

            void handleEvents(SDL_Event& event);

            bool running(){ return isRunning; }
            void setRunning(bool running) { isRunning = running; }

            const float getFps() const { return fps; }

            //StateMachine* getStateMachine() { return stateMachine.get(); }

            int getScreenWidth() const { return screenWidth; }
            int getScreenHeight() const { return screenHeight; }

            // Mainly used for Lua to get current level dimensions
            //int getMapWidth() const { return level != nullptr ? level->getWidth() : 0; }
            //int getMapHeight() const { return level != nullptr ? level->getHeight() : 0; }
            // Current level or nullptr if none
            //void setLevel(Level* lvl) { level = lvl; }
            //Level* getLevel() { return level; }

            // TODO: should be done in init() method
            void setCallbacks(
                void (*preUpdate)(float),
                void (*postUpdate)(float),
                void (*preRender)(float),
                void (*postRender)(float)) {
                preUpdateCallback = preUpdate;
                postUpdateCallback = postUpdate;
                preRenderCallback = preRender;
                postRenderCallback = postRender;
            }

            void setWindowCallbacks(void (*resizeCallback)(int, int)) {
                windowResizeCallback = resizeCallback;
            }

        private:
            Engine();
            ~Engine();

            static Engine* s_pInstance;

            void render(float deltaTime); ///< Main render method, renders current state
            void update(float deltaTime); ///< Main update method, sets different state or updates current one

            bool isRunning = false; ///< appliction running flag
            bool debugMode = false; ///< enables IMGui Debug Console
            Window window; ///< main window instance
            int screenWidth, screenHeight;

            float fps = 0; ///< main application's fps

            // Application callbacks
            void (*preUpdateCallback)(float) = nullptr;
            void (*postUpdateCallback)(float) = nullptr;
            void (*preRenderCallback)(float) = nullptr;
            void (*postRenderCallback)(float) = nullptr;

            void (*windowResizeCallback)(int, int) = nullptr;

            //std::unique_ptr<StateMachine> stateMachine = nullptr; ///< state machine's instance
            //GameState* currentState = nullptr; ///< current state's instance

            //std::vector<Entity*> entities;
            //Level* level;
    };

    typedef Engine TheEngine;
}

#endif // __Engine__
