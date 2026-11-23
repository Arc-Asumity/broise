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

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(handle_);
}

void Window::Frame()
{
    MakeCurrent();
    glClear(GL_COLOR_BUFFER_BIT);
    Update();
    Render();
    glfwSwapBuffers(handle_);
}

void Window::MakeCurrent() const
{
    glfwMakeContextCurrent(handle_);
}

void Window::Update()
{}

void Window::Render()
{}
