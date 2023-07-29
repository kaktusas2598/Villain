# Villain
2D/3D Game/Physics/Rendering Engine/Framework in development. Built using SDL2/OpenGL and C++17!

![Villain Engine](res/textures/logo_transparent.png?raw=true)

## Supported Features

### Core Engine
* Builds as a library to be used as framework or as engine editor executable
* 2D Tiled map parsing/loading from TMX/XML files
* 3D Scene loading from XML files
* Debug/Editor mode UI for scene graph editing
* Debug Rendering 2D rectangles, lines, circles, spheres, grids, bezier curves and oriented bounding boxes
* Error logging to stdout, log file and editor console
* Event Dispatcher system
* Supported Camera projection types: Perspective(First Person), Third Person,  Orthographic, Orthographic 2D or no projection
* Camera Frustum culling to increase performance
* Finite State Machine
* Resource manager with ability to set custom search directories

### Rendering
* Multi-pass forward rendering system using scene graph for 3D applications
* Phong-blinn based lighting system for Scene Graph
* Directional and Omnidirectional shadow mapping with PCF Soft shadows
* Vertex, Fragment and Geometry shader support
* 2D and Cubemap texture support
* Mipmapped textures with anisotropic filtering, 4x MultiSample Anti-Aliasing enabled
* Gamma-corrected textures
* 2D texture batch rendering, multiple texture and colour support
* Instanced rendering support for 3D models and meshes
* 3D Model loading using assimp
* Normal and Parallax mapping support
* Post-Processing filters/FX - blur, sharpen, grayscale, invert colors, edge outline (mutually exclusive ATM)
* Set optional skybox (with optional animation)
* Exponential and Layered Fog
* TrueType font rendering (Only ASCII encoding ATM)
* Sprite animation
* Skeletal animation
* Basic Nuklear UI support

### Physics
* 2D Particle Engine
* 3D Particle mass aggregate engine:
  * Particle integration using forces
  * Force generators: gravity, drag, springs, anchored springs, bungees, buoyancy, fake stiff springs
  * Contact generators: cables, rods, constraints
* 3D Rigid Body engine:
  * Rigid body Euler numerical integration
  * Force generators: gravity, spring
  * Collision Detection system:
      * Broad-Phase: Bounding Volume Hiearchy (Separate from rigid engine ATM)
      * Narrow-Phase: planes, spheres, OBBs
  * Collision Resolution using iterative contact resolver

## Planned Features

* Editor planned features:
    * Hot reloading resources
    * Saving current scene
    * Scene camera, gizmos?
* Physics Engine planned additions:
    * Friction
    * Stability improvements, optimizations
    * Constraints: rods, cables, hinges, joints
    * Terrain contact generator
    * Collision events, triggers
    * Data driven approach for particle types and emitters
* Lua Scripting
* Entity Component System?
* Environmental mapping(reflections and refractions)
* 3D Mesh batch rendering
* Tessalated terrain
* Stencil buffer
* HDR and Bloom
* Blending skeletal animations
* Embedded texture support
* Integrate terrain rendering features from 'Terrain' example

## Screenshots

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
 * [spdlog](https://github.com/gabime/spdlog)- Very fast, header-only C++ logging library
 * [lua](https://www.lua.org/) - For scripting, configuration
 * [Catch2](https://github.com/catchorg/Catch2) - Modern C++ Test Framework


## Libraries and assets used by examples
 * [Box2D](https://box2d.org/) - A 2D Physics Engine for Games
 * [Bullet Physics](https://bulletphysics.org/) - Real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc.
 * [Sponza Palace](https://github.com/jimmiebergmann/Sponza) - Sponza Palace Obj model made by Jimmie Bergmann
 * Animated models from mixamo.com


## Instructions

First install the dependencies and when use build instructions below.

### Dependencies
    sudo apt-get install -y libglew-dev
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
 * Fix Engine on Windows:
   - Linking errors for assimp
   - C++ filesystem module not compiling
   - Crashes on glBufferData?
 * Fix generating Mesh for Bullet Soft bodies: vertices.push_back makes no sense if we resize vector, also need to keep mesh updated somehow, which means
    that BulletBodyComponent needs to support btSoftBody OR create a new Node Component for soft bodies!
 * Need to keep improving normal/parallax and shadow mapping techniques. Directional shadow mapping could be better, parallax needs more testing and
   sometimes normal mapping seems to introduce UV displacement(spotted on Sponza palace walls)
 * Read about Deferred shading and decide if it's feasible to introduce it
 * Investigate alternative Anti-Aliasing methods like FXAA
 * Investigate better shadow techniques: cascaded shadow mapping and shadow volumes
 * Investigate volumetric lighting and fog techniques
 * Investigate water rendering mechanics
 * Read about Physically Based Rendering (PBR)
 * Read about Screen Space Ambient Occlusion (SSAO)
 * More testing needed for camera frustum culling, add frustum for ortho cameras, add ability to set shadow map projection plane size
 * Move all usages of <random> header to new RandomUtils static class
 * Investigate layer stack approach for rendering and handling events: layers gets renderered on top of each other with events being propagated in reverse order (from last to first)
  * Aerodynamical force generators broken or something, struggling to create flight simulator demo
  * Rework namespace system: create Villain::Physics, Villain::Rendering, etc..
  * Fix synchronisation issues between camera and models or rigid bodies
