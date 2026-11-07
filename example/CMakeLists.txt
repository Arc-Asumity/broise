add_library(TestModule SHARED
    TestModule.cpp
)

target_include_directories(TestModule PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(TestModule PRIVATE cxx_std_23)

set_target_properties(TestModule PROPERTIES
    PREFIX ""
    OUTPUT_NAME "TestModule"
    RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:Broise>/mods"
    LIBRARY_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:Broise>/mods"
)
