cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

project(Broise C CXX)

set(BROISE_DEPENDENCY_DIR "" CACHE PATH
    "Directory containing pre-cloned GLFW, GLAD, toml++ and FreeType sources")

set(PROJECT_VERSION 0.1.0)

set(PROJECT_RELEASE 1)
set(PROJECT_DESCRIPTION "A Software Runtime")

include(cmake/Deps.cmake)

add_subdirectory(src)
add_subdirectory(example)
