add_library(Broise_UI
    Font.cpp
    Rect.cpp
    Style.cpp
    Window.cpp
)
add_library(Broise::UI ALIAS Broise_UI)
set_target_properties(Broise_UI PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(Broise_UI
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_features(Broise_UI PUBLIC cxx_std_23)
target_link_libraries(Broise_UI
    PUBLIC Broise::OpenGL
    PRIVATE freetype
)
