# Villain
2D/3D Game/Physics/Rendering Engine in development.

## Supported Features

* Vertex, Fragment shader loading
* 2D and Cubemap texture support
* 3D Model loading using assimp
* Debug UI
* Error logging
* 2D orthographics and 3D perspective cameras

## Planned Features

* Sprite class, animations, batch rendering
* LUA (Again!)
* many more...

## Used Libraries

 * [OpenGL](https://www.opengl.org) - Renderer
 * [SDL2](https://www.libsdl.org/) - for window creation and input handling, sound
 * [GLEW](https://glew.sourceforge.net/) - extension library for loading OpenGL functions
 * [IMGUI](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies;
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 + [stb_image](https://github.com/nothings/stb) Public Domain Image Loading Library for C++
 * [assimp]
 * [lua]


## Instructions

First install the dependencies and when use build instructions below.

### Dependencies

### Building

Engine is built as shared library using CMake, to use it in one of your projects,
go to examples folder execute these scripts in order:
 * ./configure.sh
 * ./build.sh
 * ./run.sh

 Same scripts in the root folder will only build the engine as a library, CMakeLists.txt file can be modified by changing
 one line to build engine as an executable, but that requires having an entrypoint like int main() etc.
