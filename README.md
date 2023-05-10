# Villain
2D/3D Game/Physics/Rendering Engine/Framework in development. Built using SDL2/OpenGL and C++17!

## Supported Features

* Multi-pass forward rendering system using scene graph for 3D applications
* Phong-blinn based lighting system for Scene Graph
* Directional and Omnidirectional shadow mapping with PCF Soft shadows
* Builds as a library
* Vertex, Fragment and Geometry shader support
* 2D and Cubemap texture support
* Mipmapped textures with anisotropic filtering, 4x MultiSample Anti-Aliasing enabled
* 2D texture batch rendering, multiple texture and colour support
* 3D Model loading using assimp
* Normal and Parallax Mapping support
* Post-Processing filters/FX - blur, sharpen, grayscale, invert colors, edge outline (mutually exclusive ATM)
* 2D Tiled map parsing/loading from tmx/xml files
* 2D Particle Engine
* Debug/Edit mode UI
* Debug Rendering 2D rectangles, lines, circles, spheres and rotated 3D boxes
* Error logging to stdout, log file and editor console
* Different Camera Types: Perspective, Orthographic, Orthographic 2D or no projection
* Camera Frustum culling to increase performance
* TrueType font rendering (Only ASCII encoding ATM)
* Finite State Machine
* Sprite animation
* Basic Nuklear UI support

## Planned Features

* Engine editor with scene management
* Skeletal animation support
* Physics Engine:
    * Potential Box2D and Bullet physics integration
    * Built in collision detection: AABB, SAT, circle
    * Rigid Body implementation
    * Spatial Partitioning models
* Data Oriented Models
* LUA (Again!)
* Entity Component System
* Environmental mapping(reflections and refractions)
* Mesh batch rendering
* Stencil buffer
* Instanced drawing
* and more...
* Gamma correction/sRGB textures, HDR and Bloom


![Zombie hell](screenshots/Zombies.png?raw=true "Villain Engine Demo: 2D Bullet Hell game")
![3D features](screenshots/SponzaDemo.png?raw=true "Villain Engine Demo: 3D demo with models/lighting/shadow/normal mapping etc.")
![FPS](screenshots/FPS.png?raw=true "Villain Engine Demo: Wolfenstein/Doom clone")
![Bullet Physics](screenshots/Bullet.png?raw=true "Villain Engine Demo: Bullet Physics integration")

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


## Libraries and assets used by examples
 * [Box2D](https://box2d.org/) - A 2D Physics Engine for Games
 * [Bullet Physics](https://bulletphysics.org/) - Real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc.
 * [Sponza Palace](https://github.com/jimmiebergmann/Sponza) - Sponza Palace Obj model made by Jimmie Bergmann


## Instructions

First install the dependencies and when use build instructions below.

### Dependencies
    sudo apt-get install -y libsdl2-dev
    sudo apt-get install -y libsdl2-mixer-dev
    sudo apt-get install -y libsdl2-image-dev
    sudo apt-get install -y libsdl2-ttf-dev
    sudo apt-get install -y liblua5.4-dev
    sudo apt-get install -y libassimp-dev
    sudo apt-get install -y libbullet-dev (Optional at the moment)

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
 * Finish refactoring/porting StateParser class from Vigilant engine!
 * Look into implementing ObjectLayer class for Tiled Maps, this way we could maybe utilise Box2D..
 * Possibly refactor Logger class to be a wrapper for spdlog
 * Fix Engine on Windows:
   - NuklearUI sometimes not compiling
   - C++ filesystem module not compiling
   - Crashes on glBufferData?
 * Fix generating Mesh for Bullet Soft bodies: vertices.push_back makes no sense if we resize vector, also need to keep mesh updated somehow, which means
    that BulletBodyComponent needs to support btSoftBody OR create a new Node Component for soft bodies!
 * Need to keep improving normal/parallax and shadow mapping techniques. Directional shadow mapping could be better, parallax needs more testing and
   sometimes normal mapping seems to introduce UV displacement(spotted on Sponza palace walls)
 * Read about Deferred shading and decide if it's feasible to introduce it
 * Loading scene graph from file: XML, Lua, something else? Contain in state machine?
 * Improve and fix spot light shadows
 * Investigate alternative Anti-Aliasing methods like FXAA
 * Investigate better shadow techniques: cascaded shadow mapping and shadow volumes
 * Read about Physically Based Rendering (PBR)
 * Read about Screen Space Ambient Occlusion (SSAO)
 * Investigate terrain mesh generation and rendering
 * More testing needed for camera frustum culling, add frustum for ortho cameras, add ability to set shadow map projection plane size
 * Move all usages of <random> header to new RandomUtils static class
