add_library(Broise_Widgets MainWidget.cpp)
add_library(Broise::Widgets ALIAS Broise_Widgets)
target_include_directories(Broise_Widgets PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Broise_Widgets PUBLIC Broise::Core)
