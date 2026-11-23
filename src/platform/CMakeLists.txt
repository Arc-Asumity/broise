add_library(Broise_Platform WindowManager.cpp Window.cpp Monitor.cpp)
add_library(Broise::Platform ALIAS Broise_Platform)

target_include_directories(Broise_Platform
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(Broise_Platform
    PUBLIC
        Broise::OpenGL
        Broise::GLAD
)
