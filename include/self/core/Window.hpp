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

#include <self/core/Base.hpp>

constexpr float kScale = 2.0f / 3.0f;

enum class WindowError
{
    GlfwInitializationFailed,
    WindowCreationFailed,
    OpenGLLoaderInitializationFailed,
};

class Window {
public:
    static std::expected<Window, WindowError>
    Create(const char* title);

    ~Window();

    static void PollEvents();
    void SwapBuffers() const;

    [[nodiscard]]
    bool ShouldClose() const;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;

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
    GLFWwindow* window_{};

    int width_{};
    int height_{};

    Window() = default;

};
