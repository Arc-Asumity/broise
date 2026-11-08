// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/WindowManager.hpp
// Manages GLFW and owns application windows.

#pragma once

#include <expected>
#include <functional>
#include <vector>

#include <self/ui/Window.hpp>

class WindowManager
{
public:
    WindowManager() = default;
    ~WindowManager();

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    WindowManager(WindowManager&& other) noexcept;
    WindowManager& operator=(WindowManager&& other) noexcept;

    [[nodiscard]] std::expected<void, WindowError> InitGlfw();
    [[nodiscard]] std::expected<std::reference_wrapper<Window>, WindowError>
    CreateWindow(const char* title);
    void DestroyWindow(Window& window);
    void PollEvents() const;

private:
    void Shutdown() noexcept;

    std::vector<Window> windows_;
    bool glfw_initialized_{};
};
