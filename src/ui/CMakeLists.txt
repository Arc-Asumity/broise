add_library(Broise_UI
        Font.cpp
        Renderer.cpp
        Rect.cpp
        Style.cpp
)
add_library(Broise::UI ALIAS Broise_UI)
set_target_properties(Broise_UI PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(Broise_UI
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
