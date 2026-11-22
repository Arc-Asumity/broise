add_library(Broise_Module ModuleManager.cpp)
add_library(Broise::Module ALIAS Broise_Module)

target_include_directories(Broise_Module
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
