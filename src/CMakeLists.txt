add_executable(Broise Main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(Broise PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

add_subdirectory(parts)
add_subdirectory(modules)
add_subdirectory(core)
add_subdirectory(ui)

target_link_libraries(Broise PRIVATE Broise::Runtime)

add_custom_command(TARGET Broise POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test"
        "$<TARGET_FILE_DIR:Broise>/test"
)
