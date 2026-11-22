add_executable(Broise Main.cpp)
target_compile_features(Broise PRIVATE cxx_std_23)

add_library(Broise_ModuleAPI INTERFACE)
add_library(Broise::ModuleAPI ALIAS Broise_ModuleAPI)
target_include_directories(Broise_ModuleAPI
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_features(Broise_ModuleAPI INTERFACE cxx_std_23)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(Broise PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

add_subdirectory(core)
add_subdirectory(ui)

target_link_libraries(Broise PRIVATE Broise::Runtime)

add_custom_target(Broise_Update_Test
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test"
        "$<TARGET_FILE_DIR:Broise>"
        COMMENT "Updating resource for test..."
)
add_custom_target(Broise_Update_Lucide
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${BROISE_LUCIDE_ICONS_DIR}"
        "$<TARGET_FILE_DIR:Broise>/assets/icons"
        COMMENT "Updating Lucide icons..."
)
