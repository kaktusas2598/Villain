cmake_minimum_required(VERSION 3.25.1)

# Setup project and set name
project(VILLAIN)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
find_package(Lua51 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

file(GLOB sources src/*.cpp)
file(GLOB vendor_sources vendor/*.cpp)

# Build executable
add_executable(${PROJECT_NAME} main.cpp ${sources})

# Specify extra directories with their own CMakeLists.txt for this build
add_subdirectory(vendor/glm)
add_subdirectory(vendor/imgui)

# Build Header only libs
add_library(stb_image "vendor/stb_image.cpp")

# Set up include paths
target_include_directories(${PROJECT_NAME} PUBLIC vendor)
# Set up path to libraries
#target_link_directories(${PROJECT_NAME} PUBLIC vendor)
#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor)

# Link libraries
#file(GLOB SDL2_LIBS src/lib/*.a)
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES} ${LUA_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${GLEW_LIBRARIES} glm imgui stb_image)

# Optional for linux to install binary
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
