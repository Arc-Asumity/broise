// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Window.hpp
// Header for window.

#pragma once

#include <expected>
#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <self/ui/Base.hpp>

class Renderer;
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
    void UpdateTitleBarInput();
    void RenderTitleBar(Renderer& renderer, const Style& style) const;

    [[nodiscard]]
    bool ShouldClose() const;

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

        self->width_ = width;
        self->height_ = height;

        glViewport(0, 0, width, height);
    }

private:
    friend class WindowManager;

    GLFWwindow* window_{};
    GLFWcursor* horizontal_resize_cursor_{};
    GLFWcursor* vertical_resize_cursor_{};
    GLFWcursor* diagonal_resize_cursor_{};
    GLFWcursor* anti_diagonal_resize_cursor_{};

    int width_{};
    int height_{};
    bool left_mouse_was_down_{};
    bool dragging_title_bar_{};
    bool resizing_{};
    int resize_edges_{};
    int drag_start_window_x_{};
    int drag_start_window_y_{};
    double drag_start_cursor_x_{};
    double drag_start_cursor_y_{};
    int resize_start_width_{};
    int resize_start_height_{};

    Window() = default;

};
