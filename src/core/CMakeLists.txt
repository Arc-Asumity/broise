add_library(Broise_Runtime Application.cpp)
add_library(Broise::Runtime ALIAS Broise_Runtime)

target_include_directories(Broise_Runtime
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(Broise_Runtime
    PRIVATE
        Broise::Module
        Broise::Platform
)
