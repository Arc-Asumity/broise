// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/WindowManager.cpp
// Manages GLFW and owns application windows.

#include <algorithm>
#include <utility>

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
    if (!glfw_initialized_) {
        if (glfwInit() == GLFW_FALSE) {
            return std::unexpected(WindowError::GlfwInitializationFailed);
        }
        glfw_initialized_ = true;
    }

    auto window = Window::Create(title);
    if (!window) {
        return std::unexpected(window.error());
    }

    windows_.push_back(std::move(*window));
    return std::ref(windows_.back());
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
