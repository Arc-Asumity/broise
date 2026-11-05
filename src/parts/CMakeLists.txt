add_library(Broise_Parts RootPart.cpp)
add_library(Broise::Parts ALIAS Broise_Parts)
target_include_directories(Broise_Parts PUBLIC ${CMAKE_SOURCE_DIR}/include)
