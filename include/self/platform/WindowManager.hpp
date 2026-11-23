// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/WindowManager.hpp
// Manages GLFW and owns application windows.


#pragma once

#include <expected>
#include <string_view>
#include <vector>

#include <self/platform/Window.hpp>
#include <self/platform/Monitor.hpp>
#include <self/platform/WindowManagerError.hpp>

namespace WindowCreateParams
{
    constexpr float kScreenSizeRatio = 2.0f / 3.0f;
}

class WindowManager
{
public:
    WindowManager() = default;

    std::expected<void, WindowManagerError> Initialize(std::string_view title);
    std::expected<void, WindowManagerError> CreateWindow(std::string_view title);

    std::expected<void, WindowManagerError> Frame();
    void RemoveClosed();
    [[nodiscard]] bool ShouldClose() const;

private:
    bool glfw_initialized_ = false;
    bool glad_initialized_ = false;

    Monitor primary_monitor_{};
    const GLFWvidmode* mode_ = nullptr;

    std::vector<Window> windows_;

    std::expected<void, WindowManagerError> InitializeGlfw();
    std::expected<void, WindowManagerError> InitializeGlad();
};
