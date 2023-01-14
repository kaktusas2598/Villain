#pragma once

#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#endif

/// Main Namespace
namespace Villain {
    /**
     * Creates window with use of these flags. 0x0 to use none.
     */
    enum WindowFlags
    {
        INVISIBLE = 0x1, //<
        FULLSCREEN = 0x2, //< Fullscreen window
        BORDERLESS = 0x4 //< Borderless
    };

    /*! \brief Window
     *         Main Window Class.
     *
     *  Setups OpenGL flags and crates SDL Window, GL context.
     *  Also creates and binds VAO's.
     */
    class Window {
        public:
            Window() {}
            ~Window(){}

            /**
             * First setups GL flags, then creates SDL_Window and GL context. Finally initializes and binds VAOs.
             * @param title Title to be displayed on window title bar
             * @param screenHeight Screen height in pixels
             * @param screenWidth Screen width in pixels
             * @sa Engine::init()
             */
            void create(std::string title, int screenHeight, int screenWidth, unsigned int currentFlags);
            /**
             * Swap OpenGL framebuffers/Update Display
             */
            void swapBuffer();

            int getScreenWidth() { return m_screenWidth; } ///< Screen width getter
            int getScreenHeight() { return m_screenHeight; } ///< Screen height getter
            SDL_Window* getSDLWindow() { return m_pWindow; } ///< SDL_Window pointer getter
            void setWindowTitle(const char* title);

        private:
            SDL_Window * m_pWindow; ///< Pointer to SDL_Window
            int m_screenWidth, m_screenHeight; ///< Screen width and height
    };
}
