add_executable(Broise Main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(Broise PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

add_subdirectory(parts)
add_subdirectory(modules)
add_subdirectory(core)
add_subdirectory(modules/default)
add_dependencies(Broise Broise_DefaultModule)
target_compile_definitions(Broise PRIVATE
    "BROISE_MODULE_DIRECTORY=\"$<TARGET_FILE_DIR:Broise_DefaultModule>\"")
target_link_libraries(Broise PRIVATE Broise::Runtime)

add_custom_command(TARGET Broise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test/config"
        "$<TARGET_FILE_DIR:Broise>/test/config"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test/assets"
        "$<TARGET_FILE_DIR:Broise>/test/assets"
)
