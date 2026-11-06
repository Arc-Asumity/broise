add_library(Broise_Runtime Application.cpp Window.cpp Rect.cpp Renderer.cpp Style.cpp FontRenderer.cpp)
add_library(Broise::Runtime ALIAS Broise_Runtime)
target_include_directories(Broise_Runtime
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_link_libraries(Broise_Runtime PUBLIC Broise::OpenGL Broise::Parts
    Broise::ModuleManager
    PRIVATE tomlplusplus::tomlplusplus freetype)
