add_library(Broise_Core Application.cpp Window.cpp Rect.cpp Renderer.cpp)
add_library(Broise::Core ALIAS Broise_Core)
target_include_directories(Broise_Core PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Broise_Core PUBLIC Broise::OpenGL Broise::GLAD Broise::Widgets)
