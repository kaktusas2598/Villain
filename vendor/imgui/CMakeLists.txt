find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

file(GLOB IMGUI_SOURCE_DIR *.cpp)
add_library(imgui STATIC ${IMGUI_SOURCE_DIR})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR})

target_link_libraries(imgui
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY})
