// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Window.cpp
// Window for Broise.


#include <glad/gl.h>
#include <utility>

#include <self/core/Window.hpp>

std::expected<Window, WindowError>
Window::Create(const char* title)
{
    Window self;

    if (glfwInit() == GLFW_FALSE) {
        return std::unexpected(WindowError::GlfwInitializationFailed);
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode =
        glfwGetVideoMode(monitor);

    if (monitor == nullptr || mode == nullptr) {
        glfwTerminate();
        return std::unexpected(
            WindowError::WindowCreationFailed
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto* window = glfwCreateWindow(
        static_cast<int>(static_cast<float>(mode->width) * kScale),
        static_cast<int>(static_cast<float>(mode->height) * kScale),
        title,
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        glfwTerminate();
        return std::unexpected(WindowError::WindowCreationFailed);
    }

    int monitorX = 0;
    int monitorY = 0;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    glfwSetWindowPos(
        window,
        monitorX + static_cast<int>(static_cast<float>(mode->width) * (1 - kScale) / 2),
        monitorY + static_cast<int>(static_cast<float>(mode->height) * (1 - kScale) / 2)
    );

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return std::unexpected(
            WindowError::OpenGLLoaderInitializationFailed
        );
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(
        window,
        &framebufferWidth,
        &framebufferHeight
    );

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    self.width_ = framebufferWidth;
    self.height_ = framebufferHeight;

    glfwSetWindowUserPointer(window, &self);
    glfwSetFramebufferSizeCallback(
        window,
        FramebufferSizeCallback
    );

    self.window_ = window;

    return self;
}

Window::~Window()
{
    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(window_);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(window_);
}

Window::Window(Window&& other) noexcept
    : window_(std::exchange(other.window_, nullptr))
    , width_(other.width_)
    , height_(other.height_)
{
    if (window_ != nullptr) {
        glfwSetWindowUserPointer(window_, this);
    }
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    window_ = std::exchange(other.window_, nullptr);
    width_ = other.width_;
    height_ = other.height_;

    if (window_ != nullptr) {
        glfwSetWindowUserPointer(window_, this);
    }

    return *this;
}

int Window::GetWidth() const
{
    return width_;
}

int Window::GetHeight() const
{
    return height_;
}
