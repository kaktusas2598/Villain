#include "Window.hpp"
#include "ErrorHandler.hpp"

namespace Villain {

    void Window::create(std::string title, int screenHeight, int screenWidth, unsigned int currentFlags) {

        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;

        Uint32 flags = SDL_WINDOW_OPENGL;

        if (currentFlags & INVISIBLE) {
            flags |= SDL_WINDOW_HIDDEN;
        }
        if (currentFlags & FULLSCREEN) {
            // Mouse always focused inside window, best for games
            flags |= SDL_WINDOW_FULLSCREEN;
            // Mouse can leave window
            //flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        if (currentFlags & BORDERLESS) {
            flags |= SDL_WINDOW_BORDERLESS;
        }
        if (currentFlags & SDL_WINDOW_RESIZABLE) {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        //Use OpenGL 3.1 core
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // Enable MultiSample buffers for Multi Sample Anti-Aliasing (MSAA)
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // 4x Anti-Aliasing

        //if succeed, create the window
        m_pWindow = SDL_CreateWindow(title.c_str() , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
        //m_pWindow = SDL_CreateWindow(title.c_str() , 0, 0, screenWidth, screenHeight, flags);
        //SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN);

        if (m_pWindow == nullptr) {
            exitWithError("SDL Window could not be created :(");
        }

        // set up OpenGL context
        SDL_GLContext glContext = SDL_GL_CreateContext(m_pWindow);
        if (glContext == nullptr) {
            exitWithError("SDL_GL context could not be created");
        }
        SDL_GL_MakeCurrent(m_pWindow, glContext);

        GLenum glewStatus = glewInit();
        if (glewStatus != GLEW_OK) {
            exitWithError("Failed to initialise GLEW");
        }

        //check the OpenGL version
        printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

        // set background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //set VSYNC
        SDL_GL_SetSwapInterval(0);
    }

    void Window::swapBuffer() {
        //swap buffer and draw everything to the screen
        SDL_GL_SwapWindow(m_pWindow);
    }

    void Window::setWindowTitle(const char* title) {
        SDL_SetWindowTitle(m_pWindow, title);
    }
}
