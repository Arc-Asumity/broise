// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/Window.cpp
// Window for Broise.


#include <GLFW/glfw3.h>

#include <self/platform/Window.hpp>

Window::Window(GLFWwindow* handle)
    : handle_(handle)
{}
