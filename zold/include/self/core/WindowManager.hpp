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

#include <self/ui/window/Window.hpp>
#include <self/ui/window/CursorManager.hpp>

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
    [[nodiscard]] Window& GetWindow();
    [[nodiscard]] const Window& GetWindow() const;
    void DestroyWindow(Window& window);
    void PollEvents() const;
    void UpdateWindowInput();
    [[nodiscard]] bool ShouldClose(const Window& window) const;
    void SwapBuffers(const Window& window) const;
    void SetIcon(Window& window, const GLFWimage& icon) const;

private:
    void Shutdown() noexcept;

    std::vector<Window> windows_;
    CursorManager cursor_;
    bool glfw_initialized_{};
};
