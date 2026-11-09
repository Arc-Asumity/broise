// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/ui/window/Window.cpp
// Window for Broise.


#include <algorithm>
#include <cmath>
#include <string_view>
#include <utility>

#include <glad/gl.h>

#include <self/ui/window/Window.hpp>
#include <self/core/Renderer.hpp>
#include <self/ui/Style.hpp>

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

void Window::UpdateTitleBarInput(
    GLFWcursor* horizontal_resize_cursor,
    GLFWcursor* vertical_resize_cursor,
    GLFWcursor* diagonal_resize_cursor,
    GLFWcursor* anti_diagonal_resize_cursor)
{
    const bool left_mouse_is_down =
        glfwGetMouseButton(handle_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(handle_, &cursor_x, &cursor_y);

    int window_width = 0;
    int window_height = 0;
    glfwGetWindowSize(handle_, &window_width, &window_height);

    int window_x = 0;
    int window_y = 0;
    glfwGetWindowPos(handle_, &window_x, &window_y);

    const auto title_bar_height = static_cast<double>(GetTitleBarHeight());
    const auto control_width = static_cast<double>(GetTitleBarControlWidth());
    constexpr int kResizeLeft = 1;
    constexpr int kResizeRight = 2;
    constexpr int kResizeTop = 4;
    constexpr int kResizeBottom = 8;

    int hovered_edges = interaction_.mode == WindowInteractionMode::Resizing
        ? interaction_.resize_edges : 0;
    if (interaction_.mode != WindowInteractionMode::Resizing) {
        const double resize_border_x = 7.0 * GetWindowScaleX(handle_);
        const double resize_border_y = 7.0 * GetWindowScaleY(handle_);
        if (cursor_x < resize_border_x) hovered_edges |= kResizeLeft;
        if (cursor_x >= window_width - resize_border_x) hovered_edges |= kResizeRight;
        if (cursor_y < resize_border_y) hovered_edges |= kResizeTop;
        if (cursor_y >= window_height - resize_border_y) hovered_edges |= kResizeBottom;
    }
    GLFWcursor* cursor = nullptr;
    if ((hovered_edges & (kResizeLeft | kResizeRight)) != 0
        && (hovered_edges & (kResizeTop | kResizeBottom)) != 0) {
        const bool same_diagonal =
            ((hovered_edges & kResizeLeft) != 0)
                == ((hovered_edges & kResizeTop) != 0);
        cursor = same_diagonal ? diagonal_resize_cursor
                               : anti_diagonal_resize_cursor;
    } else if ((hovered_edges & (kResizeLeft | kResizeRight)) != 0) {
        cursor = horizontal_resize_cursor;
    } else if ((hovered_edges & (kResizeTop | kResizeBottom)) != 0) {
        cursor = vertical_resize_cursor;
    }
    glfwSetCursor(handle_, cursor);

    if (left_mouse_is_down && !interaction_.left_mouse_down) {
        interaction_.resize_edges = hovered_edges;

        if (interaction_.resize_edges != 0) {
            interaction_.mode = WindowInteractionMode::Resizing;
            interaction_.drag_start_window_x = window_x;
            interaction_.drag_start_window_y = window_y;
            interaction_.resize_start_width = window_width;
            interaction_.resize_start_height = window_height;
            interaction_.drag_start_cursor_x = static_cast<double>(window_x) + cursor_x;
            interaction_.drag_start_cursor_y = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (left_mouse_is_down && !interaction_.left_mouse_down
        && interaction_.mode != WindowInteractionMode::Resizing
        && cursor_y < title_bar_height) {
        if (cursor_x >= window_width - control_width) {
            glfwSetWindowShouldClose(handle_, GLFW_TRUE);
            status_.should_close = true;
        } else if (cursor_x >= window_width - 2.0 * control_width) {
            if (glfwGetWindowAttrib(handle_, GLFW_MAXIMIZED) == GLFW_TRUE) {
                glfwRestoreWindow(handle_);
            } else {
                glfwMaximizeWindow(handle_);
            }
        } else if (cursor_x >= window_width - 3.0 * control_width) {
            glfwIconifyWindow(handle_);
        } else {
            interaction_.mode = WindowInteractionMode::Dragging;
            interaction_.drag_start_window_x = window_x;
            interaction_.drag_start_window_y = window_y;
            interaction_.drag_start_cursor_x = static_cast<double>(window_x) + cursor_x;
            interaction_.drag_start_cursor_y = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (!left_mouse_is_down) {
        interaction_.mode = WindowInteractionMode::Idle;
        interaction_.resize_edges = 0;
    } else if (interaction_.mode == WindowInteractionMode::Resizing) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        const int delta_x = static_cast<int>(
            cursor_screen_x - interaction_.drag_start_cursor_x);
        const int delta_y = static_cast<int>(
            cursor_screen_y - interaction_.drag_start_cursor_y);

        int width = interaction_.resize_start_width;
        int height = interaction_.resize_start_height;
        int x = interaction_.drag_start_window_x;
        int y = interaction_.drag_start_window_y;
        if ((interaction_.resize_edges & kResizeRight) != 0) width += delta_x;
        if ((interaction_.resize_edges & kResizeLeft) != 0) {
            width -= delta_x;
            x += delta_x;
        }
        if ((interaction_.resize_edges & kResizeBottom) != 0) height += delta_y;
        if ((interaction_.resize_edges & kResizeTop) != 0) {
            height -= delta_y;
            y += delta_y;
        }

        const int minimum_width = static_cast<int>(
            std::lround(320.0f * GetWindowScaleX(handle_)));
        const int minimum_height = static_cast<int>(
            std::lround(200.0f * GetWindowScaleY(handle_)));
        if (width < minimum_width) {
            if ((interaction_.resize_edges & kResizeLeft) != 0) {
                x -= minimum_width - width;
            }
            width = minimum_width;
        }
        if (height < minimum_height) {
            if ((interaction_.resize_edges & kResizeTop) != 0) {
                y -= minimum_height - height;
            }
            height = minimum_height;
        }
        glfwSetWindowPos(handle_, x, y);
        glfwSetWindowSize(handle_, width, height);
    } else if (interaction_.mode == WindowInteractionMode::Dragging) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        glfwSetWindowPos(
            handle_,
            interaction_.drag_start_window_x
                + static_cast<int>(cursor_screen_x - interaction_.drag_start_cursor_x),
            interaction_.drag_start_window_y
                + static_cast<int>(cursor_screen_y - interaction_.drag_start_cursor_y)
        );
    }

    interaction_.left_mouse_down = left_mouse_is_down;
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

void Window::RenderTitleBar(Renderer& renderer, const Style& style) const
{
    const auto window_width = static_cast<float>(GetLogicalWidth());
    const auto window_height = static_cast<float>(GetLogicalHeight());
    const auto title_bar_height = static_cast<float>(GetTitleBarHeight());
    const auto control_width = static_cast<float>(GetTitleBarControlWidth());
    if (window_width <= 0.0f || window_height <= 0.0f) {
        return;
    }

    const auto make_rectangle = [window_width, window_height](
                                    const float left, const float top,
                                    const float right, const float bottom) {
        return PointSet{
            {.x = left / window_width, .y = top / window_height},
            {.x = right / window_width, .y = top / window_height},
            {.x = right / window_width, .y = bottom / window_height},
            {.x = left / window_width, .y = bottom / window_height},
        };
    };

    renderer.RenderPolygon(
        make_rectangle(0.0f, 0.0f, window_width, title_bar_height),
        style.colors.bar);

    const float close_left = window_width - control_width;
    const float maximize_left = window_width - control_width * 2.0f;
    const float minimize_left = window_width - control_width * 3.0f;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(handle_, &cursor_x, &cursor_y);
    const bool pointer_in_title_bar = cursor_y >= 0.0
        && cursor_y < title_bar_height;
    const bool minimize_hovered = pointer_in_title_bar
        && cursor_x >= minimize_left && cursor_x < maximize_left;
    const bool maximize_hovered = pointer_in_title_bar
        && cursor_x >= maximize_left && cursor_x < close_left;
    const bool close_hovered = pointer_in_title_bar
        && cursor_x >= close_left && cursor_x < window_width;
    renderer.RenderPolygon(
        make_rectangle(minimize_left, 0.0f, minimize_left + control_width,
                       title_bar_height),
        minimize_hovered ? style.colors.hover : style.colors.button);
    renderer.RenderPolygon(
        make_rectangle(maximize_left, 0.0f, close_left, title_bar_height),
        maximize_hovered ? style.colors.hover : style.colors.button);
    renderer.RenderPolygon(
        make_rectangle(close_left, 0.0f, window_width, title_bar_height),
        close_hovered ? style.colors.hover : style.colors.button);

    const float center_y = title_bar_height * 0.5f;
    const float icon_size_big = title_bar_height * 0.54f;
    const float icon_size_small = title_bar_height * 0.42f;
    renderer.RenderIcon("minus",
                        minimize_left + (control_width - icon_size_small) * 0.5f,
                        center_y - icon_size_small * 0.5f,
                        icon_size_small, icon_size_small, style.colors.text);
    renderer.RenderIcon("square",
                        maximize_left + (control_width - icon_size_small) * 0.5f,
                        center_y - icon_size_small * 0.5f,
                        icon_size_small, icon_size_small, style.colors.text);
    renderer.RenderIcon("x",
                        close_left + (control_width - icon_size_big) * 0.5f,
                        center_y - icon_size_big * 0.5f,
                        icon_size_big, icon_size_big, style.colors.text);
}
