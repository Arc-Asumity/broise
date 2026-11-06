add_library(Broise_Parts
    RootPart.cpp
    TitleBarPart.cpp
    RectanglePart.cpp
    TextPart.cpp
    ButtonPart.cpp
)
add_library(Broise::Parts ALIAS Broise_Parts)
target_include_directories(Broise_Parts
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
