add_library(Broise_Assets Icon.cpp)
add_library(Broise::Assets ALIAS Broise_Assets)

target_include_directories(Broise_Assets
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(Broise_Assets
    PRIVATE
        lunasvg
)
