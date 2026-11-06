add_library(Broise_DefaultModule MODULE
    DefaultModule.cpp
    ClockWidget.cpp
)
add_library(Broise::DefaultModule ALIAS Broise_DefaultModule)
target_link_libraries(Broise_DefaultModule
    PRIVATE
        Broise::ModuleAPI
        Broise::Runtime
)
target_include_directories(Broise_DefaultModule
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_definitions(Broise_DefaultModule PRIVATE BROISE_MODULE_BUILD)
target_compile_features(Broise_DefaultModule PRIVATE cxx_std_23)
set_target_properties(Broise_DefaultModule PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins"
)
