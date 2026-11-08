
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    lucide
    GIT_REPOSITORY https://github.com/lucide-icons/lucide.git
    GIT_TAG 1.62.0
    GIT_SHALLOW TRUE
)
FetchContent_Populate(
        lucide
        SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/lucide-src"
        GIT_REPOSITORY https://github.com/lucide-icons/lucide.git
        GIT_TAG 1.62.0
        GIT_SHALLOW TRUE
)

set(BROISE_LUCIDE_ICONS_DIR "${lucide_SOURCE_DIR}/icons")

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad)

glad_add_library(
        Broise_GLAD
        STATIC
        REPRODUCIBLE
        API gl:core=4.6
)
add_library(Broise::GLAD ALIAS Broise_GLAD)

find_package(OpenGL REQUIRED)

FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(tomlplusplus)

set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_BROTLI ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_HVF ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_ZLIB ON)

FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-14-3
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(freetype)

add_library(Broise_OpenGL INTERFACE)
target_link_libraries(
        Broise_OpenGL
        INTERFACE
        glfw
        OpenGL::GL
        Broise::GLAD
)
add_library(Broise::OpenGL ALIAS Broise_OpenGL)
