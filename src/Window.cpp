#include "Window.hpp"
#include "ErrorHandler.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

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

        GLenum glewStatus = glewInit();
        if (glewStatus != GLEW_OK) {
            exitWithError("Failed to initialise GLEW");
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(m_pWindow, glContext);
        ImGui_ImplOpenGL3_Init("#version 330");

        //check the OpenGL version
        printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

        // set background color
        glClearColor(255.0f, 0.0f, 0.0f, 1.0f);

        //set VSYNC
        SDL_GL_SetSwapInterval(0);

        //enable alpha blending
        glEnable(GL_BLEND);
        //what kind of blending we want
        //in this case, we want alpha 0 to be transparent
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Window::swapBuffer() {
        //swap buffer and draw everything to the screen
        SDL_GL_SwapWindow(m_pWindow);
    }

    void Window::setWindowTitle(const char* title) {
        SDL_SetWindowTitle(m_pWindow, title);
    }
}
