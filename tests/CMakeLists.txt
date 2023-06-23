cmake_minimum_required(VERSION 3.20)

project(VillainTests)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Enable code coverage flags for GCC
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_subdirectory(../vendor/catch2 out)

# Add engine as library, send lib file to this project's dir
add_subdirectory(../ ${PROJECT_SOURCE_DIR})

include_directories(../src ../vendor/catch2/src)

file(GLOB tests unit/*)
add_executable(${PROJECT_NAME} ${tests})

target_link_libraries(${PROJECT_NAME} PRIVATE VILLAIN Catch2::Catch2WithMain)

# Add a custom target for generating the coverage report
add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND} -j ${PROCESSOR_COUNT}
    COMMAND ${CMAKE_COMMAND} -E env lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND ${CMAKE_COMMAND} -E env genhtml ${CMAKE_BINARY_DIR}/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage-report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Register the tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
