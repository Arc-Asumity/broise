// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/window/Window.hpp
// Header for window.

#pragma once

#include <expected>
#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <self/ui/Base.hpp>
#include <self/ui/window/CursorManager.hpp>
#include <self/ui/window/WindowInteraction.hpp>
#include <self/ui/window/WindowMetrics.hpp>
#include <self/ui/window/WindowStatus.hpp>
#include <self/ui/window/WindowContent.hpp>

class Renderer;
class WindowContent;
class WindowManager;
struct Style;

constexpr float kScreenSizeRatio = 2.0f / 3.0f;

enum class WindowError
{
    GlfwInitializationFailed,
    WindowCreationFailed,
    OpenGLLoaderInitializationFailed,
    StyleConfigurationFailed,
};

class Window {
public:
    ~Window();

    void SwapBuffers() const;
    void SetIcon(const GLFWimage& icon) const;
    void UpdateTitleBarInput(CursorManager& cursor_manager);

    [[nodiscard]]
    bool ShouldClose() const;
    [[nodiscard]] WindowStatus GetStatus() const;
    [[nodiscard]] WindowMetrics GetMetrics() const;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] int GetLogicalWidth() const;
    [[nodiscard]] int GetLogicalHeight() const;
    [[nodiscard]] int GetTitleBarHeight() const;
    [[nodiscard]] int GetTitleBarControlWidth() const;

    static void FramebufferSizeCallback(
        GLFWwindow* window, const int width, const int height)
    {
        auto* self = static_cast<Window*>(
            glfwGetWindowUserPointer(window));
        if (!self) return;

        self->metrics_.framebuffer_width = width;
        self->metrics_.framebuffer_height = height;

        glViewport(0, 0, width, height);
    }

private:
    friend class WindowManager;
    friend class WindowContent;
    friend class Renderer;

    GLFWwindow* handle_{};
    WindowStatus status_{};
    WindowMetrics metrics_{};
    WindowInteraction interaction_{};
    WindowContent content_{};

    Window() = default;

};
