add_executable(Broise Main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(Broise PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

target_include_directories(Broise PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Broise PRIVATE Broise::Core)

add_subdirectory(core)
