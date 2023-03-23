# Villain
2D/3D Game/Physics/Rendering Engine/Framework in development.

## Supported Features

* Vertex, Fragment shader loading
* 2D and Cubemap texture support
* 2D texture batch rendering, multiple texture and colour support
* 3D Model loading using assimp
* 2D Tiled map parsing/loading from tmx/xml files
* 2D Particle Engine
* Debug UI
* Error logging
* 2D orthographics and 3D perspective cameras
* TrueType font rendering (Only ASCII encoding ATM)
* Finite State Machine
* Sprite animation

## Planned Features

* Skeletal animation support
* Built in collision detection: AABB, SAT, circle
* Rigid Body implementation
* Scene Graphs
* Spatial Partitioning models
* Data Oriented Models
* LUA (Again!)
* Entity Component System
* UI
* Phong based lighting system
* and many more...

## Used Libraries

 * [OpenGL](https://www.opengl.org) - Renderer
 * [SDL2](https://www.libsdl.org/) - for window creation and input handling, sound
 * [GLEW](https://glew.sourceforge.net/) - extension library for loading OpenGL functions
 * [GLM](https://glm.g-truc.net/0.9.8/index.html) - C++ Mathematics library for graphics applications.
 * [ImGui](https://github.com/ocornut/imgui) - Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies (for engine tooling)
 * [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) - Minimal-state, immediate mode GUI written in C (for app ui)
 * [stb_image](https://github.com/nothings/stb) - Public Domain Image Loading Library for C++
 * [FreeType](https://freetype.org/index.html) - freely available software library to render fonts
 * [tinyxml2](https://github.com/leethomason/tinyxml2) - Simple, small, efficient C++ XML parser
 * [assimp](https://github.com/assimp/assimp) - Open Asset Importer Library for loading 3D models
 * [lua](https://www.lua.org/) - For scripting, configuration

## Libraries used by examples
 * [Box2D](https://box2d.org/) - A 2D Physics Engine for Games


## Instructions

First install the dependencies and when use build instructions below.

### Dependencies
    sudo apt-get install -y libsdl2-dev
    sudo apt-get install -y libsdl2-mixer-dev
    sudo apt-get install -y libsdl2-image-dev
    sudo apt-get install -y libsdl2-ttf-dev
    sudo apt-get install -y liblua5.4-dev
    sudo apt-get install -y libassimp-dev

### Building

Engine is built as shared library using CMake, to use it in one of your projects,
go to examples folder execute these scripts in order:
 * ./configure.sh
 * ./build.sh
 * ./run.sh

 Same scripts in the root folder will only build the engine as a library, CMakeLists.txt file can be modified by changing
 one line to build engine as an executable, but that requires having an entrypoint like int main() etc.


## Dev Notes

I use neovim with clangd LSP server for editing code. To build compilation database for clangd, so I can get great autocompletion
use [bear](https://github.com/rizsotto/Bear). I also use [CMake](https://cmake.org/) for build

## FIXME

 * SpriteFont class - all glyphs are packed to single GL texture for performance and their sizes
     seem correct, but glyphs themselves are rendering weirdly
 * FreeType class - font rendering works just fine, but it would be better if all glyphs were packed
     in a single texture before drawing
 * Improve DebugRenderer: add lines and 3d primitive draw calls, use unique_ptr abstractions like VertexBuffer class
 * Refactor Renderer class to draw without index buffer object
 * Finish refactoring/porting StateParser class from Vigilant engine!
 * Look into implementing ObjectLayer class for Tiled Maps, this way we could maybe utilise Box2D..
 * Refactor DebugConsole class and separate responsibilities: ImGUI LuaConsole, ImGui Init, dockspaces..

## GL TODOS

 * Geometry, tessalation shader support
 * Instanced draw calls
 * Framebuffer and stencil buffer support for some cool effects


