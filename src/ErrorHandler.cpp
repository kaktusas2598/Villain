#include "ErrorHandler.hpp"

#include <iostream>
#include <SDL2/SDL.h>

void exitWithError(std::string error)
{
    std::cout << error << std::endl;

    SDL_Quit();
    exit(80);
}

void GLClearError() {
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << "(" << error << "): " << func << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}
