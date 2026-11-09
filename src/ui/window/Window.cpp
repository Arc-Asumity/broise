// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/ui/window/Window.cpp
// Window for Broise.


#include <cmath>
#include <utility>

#include <glad/gl.h>

#include <self/ui/window/Window.hpp>

namespace
{
    float GetWindowScaleX(GLFWwindow* window)
    {
        float x_scale = 1.0f;
        float y_scale = 1.0f;
        glfwGetWindowContentScale(window, &x_scale, &y_scale);
        return x_scale > 0.0f ? x_scale : 1.0f;
    }

    float GetWindowScaleY(GLFWwindow* window)
    {
        float x_scale = 1.0f;
        float y_scale = 1.0f;
        glfwGetWindowContentScale(window, &x_scale, &y_scale);
        return y_scale > 0.0f ? y_scale : 1.0f;
    }
}

Window::~Window()
{
    if (handle_ != nullptr)
    {
        glfwDestroyWindow(handle_);
    }
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(handle_);
}

void Window::SetIcon(const GLFWimage& icon) const
{
    glfwSetWindowIcon(handle_, 1, &icon);
}

void Window::RenderContent(Renderer& renderer, const Style& style) const
{
    content_.Render(*this, renderer, style);
}

void Window::UpdateTitleBarInput(CursorManager& cursor_manager)
{
    content_.UpdateTitleBarInput(*this, cursor_manager);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(handle_);
}

WindowStatus Window::GetStatus() const
{
    WindowStatus status = status_;
    status.focused = glfwGetWindowAttrib(handle_, GLFW_FOCUSED) == GLFW_TRUE;
    status.minimized = glfwGetWindowAttrib(handle_, GLFW_ICONIFIED) == GLFW_TRUE;
    status.maximized = glfwGetWindowAttrib(handle_, GLFW_MAXIMIZED) == GLFW_TRUE;
    status.visible = glfwGetWindowAttrib(handle_, GLFW_VISIBLE) == GLFW_TRUE;
    status.should_close = glfwWindowShouldClose(handle_) == GLFW_TRUE;
    return status;
}

WindowMetrics Window::GetMetrics() const
{
    WindowMetrics metrics = metrics_;
    glfwGetWindowSize(handle_, &metrics.logical_width, &metrics.logical_height);
    glfwGetFramebufferSize(handle_, &metrics.framebuffer_width,
                           &metrics.framebuffer_height);
    glfwGetWindowContentScale(handle_, &metrics.content_scale_x,
                              &metrics.content_scale_y);
    return metrics;
}

Window::Window(Window&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr))
    , status_(other.status_)
    , metrics_(other.metrics_)
    , interaction_(other.interaction_)
{
    if (handle_ != nullptr) {
        glfwSetWindowUserPointer(handle_, this);
    }
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (handle_ != nullptr) {
        glfwDestroyWindow(handle_);
    }

    handle_ = std::exchange(other.handle_, nullptr);
    status_ = other.status_;
    metrics_ = other.metrics_;
    interaction_ = other.interaction_;

    if (handle_ != nullptr) {
        glfwSetWindowUserPointer(handle_, this);
    }

    return *this;
}

int Window::GetWidth() const
{
    return metrics_.framebuffer_width;
}

int Window::GetHeight() const
{
    return metrics_.framebuffer_height;
}

int Window::GetLogicalWidth() const
{
    int width = 0;
    int height = 0;
    glfwGetWindowSize(handle_, &width, &height);
    return width;
}

int Window::GetLogicalHeight() const
{
    int width = 0;
    int height = 0;
    glfwGetWindowSize(handle_, &width, &height);
    return height;
}

int Window::GetTitleBarHeight() const
{
    constexpr float kDesignTitleBarHeight = 32.0f;
    return static_cast<int>(std::lround(
        kDesignTitleBarHeight * GetWindowScaleY(handle_)));
}

int Window::GetTitleBarControlWidth() const
{
    constexpr float kDesignTitleBarControlWidth = 48.0f;
    return static_cast<int>(std::lround(
        kDesignTitleBarControlWidth * GetWindowScaleX(handle_)));
}
