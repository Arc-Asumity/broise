// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/WindowManager.cpp
// Manages GLFW and owns application windows.


#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <self/platform/WindowManager.hpp>

std::expected<void, WindowManagerError> WindowManager::Initialize(const std::string_view title)
{
    if (auto result = InitializeGlfw(); !result)
        return result;
    if (auto result = CreateWindow(title); !result)
        return result;
    if (auto result = InitializeGlad(); !result)
        return result;
    return {};
}

std::expected<void, WindowManagerError> WindowManager::CreateWindow(std::string_view title)
{
    auto* native_window = glfwCreateWindow(
        static_cast<int>(static_cast<float>(primary_monitor_.width) * WindowCreateParams::kScreenSizeRatio),
        static_cast<int>(static_cast<float>(primary_monitor_.height) * WindowCreateParams::kScreenSizeRatio),
        title.data(), nullptr, nullptr);
    if (native_window == nullptr)
        return std::unexpected(WindowManagerError::WindowCreationFailed);

    glfwSetWindowPos(
        native_window,
        primary_monitor_.position_x + static_cast<int>(static_cast<float>(primary_monitor_.width) * (1 - WindowCreateParams::kScreenSizeRatio) / 2),
        primary_monitor_.position_x + static_cast<int>(static_cast<float>(primary_monitor_.height) * (1 - WindowCreateParams::kScreenSizeRatio) / 2));

    glfwMakeContextCurrent(native_window);
    glfwSwapInterval(1);

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(native_window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    //window.metrics_.framebuffer_width = framebuffer_width;
    //window.metrics_.framebuffer_height = framebuffer_height;

    //glfwSetWindowUserPointer(native_window, &window);
    //glfwSetFramebufferSizeCallback(
    //    native_window, Window::FramebufferSizeCallback);

    windows_.emplace_back(native_window);
    return {};
}

std::expected<void, WindowManagerError> WindowManager::Frame()
{
    glfwPollEvents();
    for (Window window : windows_)
        window.Frame();
}

void WindowManager::RemoveClosed()
{
    auto it = windows_.begin();
    while (it != windows_.end())
    {
        if (it->ShouldClose())
            it = windows_.erase(it);
        else
            ++it;
    }
}

bool WindowManager::ShouldClose() const
{
    return windows_.empty();
}

std::expected<void, WindowManagerError> WindowManager::InitializeGlfw()
{
    if (glfw_initialized_) return {};
    glfw_initialized_ = true;

    if (glfwInit() == GLFW_FALSE)
        return std::unexpected(WindowManagerError::GlfwInitializationFailed);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* monitor_ptr = glfwGetPrimaryMonitor();
    if (monitor_ptr == nullptr)
        return std::unexpected(WindowManagerError::PrimaryMonitorUnavailable);

    if (auto result = QueryMonitor(monitor_ptr); result)
        primary_monitor_ = result;
    else
        return std::unexpected(result);

    return {};
}

std::expected<void, WindowManagerError> WindowManager::InitializeGlad()
{
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        glfwDestroyWindow(glfwGetCurrentContext());
        return std::unexpected(WindowManagerError::GladInitializationFailed);
    }
    return {};
}
