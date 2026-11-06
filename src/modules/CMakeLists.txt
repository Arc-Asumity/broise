add_library(Broise_ModuleAPI INTERFACE)
add_library(Broise::ModuleAPI ALIAS Broise_ModuleAPI)
target_include_directories(Broise_ModuleAPI
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_features(Broise_ModuleAPI INTERFACE cxx_std_23)

add_library(Broise_ModuleManager STATIC
    DynamicLibrary.cpp
    ModuleManager.cpp
)
set_target_properties(Broise_ModuleManager PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(Broise::ModuleManager ALIAS Broise_ModuleManager)
target_include_directories(Broise_ModuleManager
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_link_libraries(Broise_ModuleManager PUBLIC Broise::ModuleAPI)
if (CMAKE_DL_LIBS)
    target_link_libraries(Broise_ModuleManager PRIVATE ${CMAKE_DL_LIBS})
endif ()
