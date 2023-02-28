cmake_minimum_required(VERSION 3.22.1)

# Setup project and set name
project(2D_Example)

# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Build executable
add_executable(${PROJECT_NAME} WIN32 main.cpp Bullet.cpp)

# Add engine as library, send lib file to this project's dir
add_subdirectory(../../ ${PROJECT_SOURCE_DIR})

# Set up include paths
include_directories(${LUA_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ../../)
# Set up path to libraries
target_link_directories(${PROJECT_NAME} PUBLIC ../../)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC VILLAIN)
