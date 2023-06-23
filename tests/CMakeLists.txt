cmake_minimum_required(VERSION 3.20)

project(VillainTests)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(catch2)

# Add engine as library, send lib file to this project's dir
add_subdirectory(../ ${PROJECT_SOURCE_DIR})

include_directories(../src catch2/src)

add_executable(${PROJECT_NAME} sample_test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE VILLAIN Catch2::Catch2WithMain)

# Register the tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
