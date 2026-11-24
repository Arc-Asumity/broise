// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/Window.cpp
// Window for Broise.


#include <utility>
#include <GLFW/glfw3.h>

#include <self/platform/Window.hpp>

Window::Window(GLFWwindow* handle)
    : handle_(handle)
{}

Window::~Window()
{
    glfwDestroyWindow(handle_);
}

Window::Window(Window&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr))
{
    glfwSetWindowUserPointer(handle_, this);
}

Window& Window::operator=(Window&& other) noexcept
{
    glfwDestroyWindow(handle_);

    handle_ = std::exchange(other.handle_, nullptr);
    glfwSetWindowUserPointer(handle_, this);

    return *this;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(handle_);
}

void Window::Frame() const
{
    MakeCurrent();
    glClear(GL_COLOR_BUFFER_BIT);
    Render();
    glfwSwapBuffers(handle_);
}

void Window::MakeCurrent() const
{
    glfwMakeContextCurrent(handle_);
}

void Window::Update()
{}

void Window::Render() const
{}

void Window::SetIcon(const GLFWimage& icon)
{}
