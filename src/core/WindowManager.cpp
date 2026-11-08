// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/WindowManager.cpp
// Manages GLFW and owns application windows.


#include <algorithm>
#include <utility>

#include <glad/gl.h>

#include <self/core/WindowManager.hpp>

WindowManager::~WindowManager()
{
    Shutdown();
}

WindowManager::WindowManager(WindowManager&& other) noexcept
    : windows_(std::move(other.windows_))
    , glfw_initialized_(std::exchange(other.glfw_initialized_, false))
{}

WindowManager& WindowManager::operator=(WindowManager&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    const bool other_initialized = other.glfw_initialized_;
    windows_.clear();
    if (glfw_initialized_ && !other_initialized) {
        glfwTerminate();
        glfw_initialized_ = false;
    }
    windows_ = std::move(other.windows_);
    glfw_initialized_ = glfw_initialized_
        || std::exchange(other.glfw_initialized_, false);
    return *this;
}

std::expected<std::reference_wrapper<Window>, WindowError>
WindowManager::CreateWindow(const char* title)
{
    if (auto initialized = InitGlfw(); !initialized) {
        return std::unexpected(initialized.error());
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (monitor == nullptr || mode == nullptr) {
        return std::unexpected(WindowError::WindowCreationFailed);
    }

    auto* native_window = glfwCreateWindow(
        static_cast<int>(static_cast<float>(mode->width) * kScreenSizeRatio),
        static_cast<int>(static_cast<float>(mode->height) * kScreenSizeRatio),
        title, nullptr, nullptr);
    if (native_window == nullptr) {
        return std::unexpected(WindowError::WindowCreationFailed);
    }

    Window window;
    window.horizontal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    window.vertical_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    window.diagonal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    window.anti_diagonal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

    int monitor_x = 0;
    int monitor_y = 0;
    glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);
    glfwSetWindowPos(
        native_window,
        monitor_x + static_cast<int>(mode->width * (1 - kScreenSizeRatio) / 2),
        monitor_y + static_cast<int>(mode->height * (1 - kScreenSizeRatio) / 2));

    glfwMakeContextCurrent(native_window);
    glfwSwapInterval(1);
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        glfwDestroyWindow(native_window);
        return std::unexpected(WindowError::OpenGLLoaderInitializationFailed);
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(native_window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    window.width_ = framebuffer_width;
    window.height_ = framebuffer_height;
    window.window_ = native_window;
    glfwSetWindowUserPointer(native_window, &window);
    glfwSetFramebufferSizeCallback(
        native_window, Window::FramebufferSizeCallback);

    windows_.push_back(std::move(window));
    return std::ref(windows_.back());
}

std::expected<void, WindowError> WindowManager::InitGlfw()
{
    if (glfw_initialized_) {
        return {};
    }
    if (glfwInit() == GLFW_FALSE) {
        return std::unexpected(WindowError::GlfwInitializationFailed);
    }

    glfw_initialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    return {};
}

void WindowManager::DestroyWindow(Window& window)
{
    const auto it = std::ranges::find_if(
        windows_, [&window](const Window& owned_window) {
            return &owned_window == &window;
        });
    if (it != windows_.end()) {
        windows_.erase(it);
    }
}

void WindowManager::PollEvents() const
{
    if (glfw_initialized_) {
        glfwPollEvents();
    }
}

void WindowManager::Shutdown() noexcept
{
    windows_.clear();
    if (glfw_initialized_) {
        glfwTerminate();
        glfw_initialized_ = false;
    }
}
