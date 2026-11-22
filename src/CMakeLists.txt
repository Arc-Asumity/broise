add_executable(Broise Main.cpp)
target_compile_features(Broise PRIVATE cxx_std_23)

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(platform)

target_link_libraries(Broise
    PRIVATE
        Broise::Runtime
        Broise::Platform
)
