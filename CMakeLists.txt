cmake_minimum_required(VERSION 3.22.1)

# Setup project and set name
project(VILLAIN)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Append to cmake module dir and add files like FindSDL2.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(Lua REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})
message(STATUS "Lua include dir: ${LUA_INCLUDE_DIR}")
message(STATUS "SDL2 include dir: ${SDL2_INCLUDE_DIR}")
message(STATUS "Glew include dir: ${GLEW_INCLUDE_DIRS}")
message(STATUS "Freetype include dir: ${FREETYPE_INCLUDE_DIRS}")

file(GLOB sources src/*.cpp)

# Uncoment to build executable, but make sure to comment add_library() line
#add_executable(${PROJECT_NAME} main.cpp ${sources})

# Build as library
# NV: On WSL I had to change shared to static to make this work, whyyyyy???
add_library(${PROJECT_NAME} STATIC ${sources})


# Specify extra directories with their own CMakeLists.txt for this build
add_subdirectory(vendor/glm)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/tinyxml2)

# Build Header only libs
add_library(stb_image "vendor/stb_image.cpp")

# Set up include paths
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_include_directories(${PROJECT_NAME} PUBLIC vendor)
target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})
# Set up path to libraries
#target_link_directories(${PROJECT_NAME} PUBLIC vendor)
#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${assimp_LIBRARIES} ${LUA_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${OpenGL_LIBRARIES} ${GLEW_LIBRARIES} ${CMAKE_DL_LIBS} glm imgui stb_image tinyxml2 ${FREETYPE_LIBRARIES})

# Optional for linux to install binary
# To actually install use "sudo make install"
# Install lib
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
# Install includes
file(GLOB includes src/*.hpp)
install(FILES ${includes} DESTINATION include)
