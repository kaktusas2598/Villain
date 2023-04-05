# Villain
2D/3D Game/Physics/Rendering Engine/Framework in development. Built using SDL2/OpenGL and C++17!

## Supported Features

* Multi-pass forward rendering system using scene graph for 3D applications
* Phong based lighting system for Scene Graph
* Builds as a library
* Vertex, Fragment and Geometry shader support
* 2D and Cubemap texture support
* 2D texture batch rendering, multiple texture and colour support
* 3D Model loading using assimp
* 2D Tiled map parsing/loading from tmx/xml files
* 2D Particle Engine
* Debug/Edit mode UI
* Error logging to stdout, log file and editor console
* 2D orthographics and 3D perspective cameras
* Camera Frustum culling to increase performance
* TrueType font rendering (Only ASCII encoding ATM)
* Finite State Machine
* Sprite animation
* Basic Nuklear UI support

## Planned Features

* Engine editor with scene management
* Skeletal animation support
* Built in collision detection: AABB, SAT, circle
* Rigid Body implementation
* Spatial Partitioning models
* Data Oriented Models
* LUA (Again!)
* Entity Component System
* Environmental mapping(reflections and refractions)
* Shadow mapping and normal mapping
* Stencil buffer
* Instanced drawing
* and many more...


![2D example](screenshots/Zombies.png?raw=true "Villain Engine Demo: 2D Bullet Hell game")
![3D example](screenshots/3Dimgui.png?raw=true "Villain Engine Demo: 3D phong lighting/skybox demo")

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

## FIXME/TODOS

 * SpriteFont class - all glyphs are packed to single GL texture for performance and their sizes
     seem correct, but glyphs themselves are rendering weirdly
 * FreeType class - font rendering works just fine, but it would be better if all glyphs were packed
     in a single texture before drawing
 * Improve DebugRenderer: add lines and 3d primitive draw calls, use unique_ptr abstractions like VertexBuffer class
 * Add ability to render spheres in DebugRenderer
 * Refactor Renderer class to draw without index buffer object
 * Finish refactoring/porting StateParser class from Vigilant engine!
 * Look into implementing ObjectLayer class for Tiled Maps, this way we could maybe utilise Box2D..
 * Possibly refactor Logger class to be a wrapper for spdlog
 * Implement Normal Mapping ( currently doing that, but not working, do I need gamma correction?)
 * Investigate shadow mapping techniques
 * Beyond camera frustum culling(done), find more ways to optimise performance on a large model (sponza palace)
 * SpotLight could take Camera* as an optional param to make it act as a flashlight
 * Improve Phong shading by implementing Blinn-Phong shading
 * Investigate gamma correction and sRGB textures

