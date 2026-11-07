add_library(Broise_ModuleAPI INTERFACE)
add_library(Broise::ModuleAPI ALIAS Broise_ModuleAPI)
target_include_directories(Broise_ModuleAPI
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_features(Broise_ModuleAPI INTERFACE cxx_std_23)

add_library(Broise_ModuleSupport STATIC
    DynamicLibrary.cpp
)
set_target_properties(Broise_ModuleSupport PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(Broise::ModuleSupport ALIAS Broise_ModuleSupport)
target_compile_features(Broise_ModuleSupport PUBLIC cxx_std_23)
target_include_directories(Broise_ModuleSupport
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_link_libraries(Broise_ModuleSupport PUBLIC Broise::ModuleAPI)
if (CMAKE_DL_LIBS)
    target_link_libraries(Broise_ModuleSupport PRIVATE ${CMAKE_DL_LIBS})
endif ()
