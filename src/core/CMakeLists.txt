add_library(Broise_Runtime SHARED
    Application.cpp
    IconManager.cpp
    ModuleLoader.cpp
    WidgetManager.cpp
    WindowManager.cpp
    Renderer.cpp
)
add_library(Broise::Runtime ALIAS Broise_Runtime)
get_filename_component(BROISE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/.." ABSOLUTE)
set_target_properties(Broise_Runtime PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_target_properties(Broise_Runtime PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BROISE_RUNTIME_OUTPUT_DIRECTORY}")
target_compile_features(Broise_Runtime PUBLIC cxx_std_23)
target_include_directories(Broise_Runtime
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_link_libraries(Broise_Runtime
    PUBLIC
        Broise::OpenGL
        Broise::ModuleAPI
    PRIVATE
        Broise::UI
        lunasvg::lunasvg
)
if (CMAKE_DL_LIBS)
    target_link_libraries(Broise_Runtime PRIVATE ${CMAKE_DL_LIBS})
endif ()
