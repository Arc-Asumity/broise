// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/platform/Window.hpp
// Header for window.


#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

class Window {
public:
    explicit Window(GLFWwindow* handle);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    [[nodiscard]] bool ShouldClose() const;

    void Frame() const;
    void Update();

private:
    GLFWwindow* handle_{};

    void MakeCurrent() const;
    void Render() const;
    void SetIcon(const GLFWimage& icon);
};
