// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/ui/window/WindowContent.cpp
// Renders the visual content owned by a window.

#include <self/ui/window/WindowContent.hpp>

#include <cmath>

#include <self/core/Renderer.hpp>
#include <self/ui/Style.hpp>
#include <self/ui/window/Window.hpp>

void WindowContent::Render(
    Window& window, Renderer& renderer, const Style& style) const
{
    const auto window_width = static_cast<float>(window.GetLogicalWidth());
    const auto window_height = static_cast<float>(window.GetLogicalHeight());
    const auto title_bar_height = static_cast<float>(window.GetTitleBarHeight());
    const auto control_width = static_cast<float>(window.GetTitleBarControlWidth());
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
    glfwGetCursorPos(window.handle_, &cursor_x, &cursor_y);
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
                        minimize_left + (control_width - icon_size_big) * 0.5f,
                        center_y - icon_size_big * 0.5f,
                        icon_size_big, icon_size_big, style.colors.text);
    renderer.RenderIcon(window.GetStatus().maximized ? "copy" : "square",
                        maximize_left + (control_width - icon_size_small) * 0.5f,
                        center_y - icon_size_small * 0.5f,
                        icon_size_small, icon_size_small, style.colors.text);
    renderer.RenderIcon("x",
                        close_left + (control_width - icon_size_big) * 0.5f,
                        center_y - icon_size_big * 0.5f,
                        icon_size_big, icon_size_big, style.colors.text);
}

namespace
{
    float GetWindowScaleX(GLFWwindow* handle)
    {
        float x_scale = 1.0f;
        float y_scale = 1.0f;
        glfwGetWindowContentScale(handle, &x_scale, &y_scale);
        return x_scale > 0.0f ? x_scale : 1.0f;
    }

    float GetWindowScaleY(GLFWwindow* handle)
    {
        float x_scale = 1.0f;
        float y_scale = 1.0f;
        glfwGetWindowContentScale(handle, &x_scale, &y_scale);
        return y_scale > 0.0f ? y_scale : 1.0f;
    }
}

void WindowContent::UpdateTitleBarInput(Window& window, CursorManager& cursor_manager)
{
    const bool left_mouse_is_down =
        glfwGetMouseButton(window.handle_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window.handle_, &cursor_x, &cursor_y);

    int window_width = 0;
    int window_height = 0;
    glfwGetWindowSize(window.handle_, &window_width, &window_height);

    int window_x = 0;
    int window_y = 0;
    glfwGetWindowPos(window.handle_, &window_x, &window_y);

    const auto title_bar_height = static_cast<double>(window.GetTitleBarHeight());
    const auto control_width = static_cast<double>(window.GetTitleBarControlWidth());
    constexpr int kResizeLeft = 1;
    constexpr int kResizeRight = 2;
    constexpr int kResizeTop = 4;
    constexpr int kResizeBottom = 8;

    int hovered_edges = window.interaction_.mode == WindowInteractionMode::Resizing
        ? window.interaction_.resize_edges : 0;
    if (window.interaction_.mode != WindowInteractionMode::Resizing) {
        const double resize_border_x = 7.0 * GetWindowScaleX(window.handle_);
        const double resize_border_y = 7.0 * GetWindowScaleY(window.handle_);
        if (cursor_x < resize_border_x) hovered_edges |= kResizeLeft;
        if (cursor_x >= window_width - resize_border_x) hovered_edges |= kResizeRight;
        if (cursor_y < resize_border_y) hovered_edges |= kResizeTop;
        if (cursor_y >= window_height - resize_border_y) hovered_edges |= kResizeBottom;
    }
    cursor_manager.Apply(window.handle_, hovered_edges);

    if (left_mouse_is_down && !window.interaction_.left_mouse_down) {
        window.interaction_.resize_edges = hovered_edges;

        if (window.interaction_.resize_edges != 0) {
            window.interaction_.mode = WindowInteractionMode::Resizing;
            window.interaction_.drag_start_window_x = window_x;
            window.interaction_.drag_start_window_y = window_y;
            window.interaction_.resize_start_width = window_width;
            window.interaction_.resize_start_height = window_height;
            window.interaction_.drag_start_cursor_x = static_cast<double>(window_x) + cursor_x;
            window.interaction_.drag_start_cursor_y = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (left_mouse_is_down && !window.interaction_.left_mouse_down
        && window.interaction_.mode != WindowInteractionMode::Resizing
        && cursor_y < title_bar_height) {
        if (cursor_x >= window_width - control_width) {
            glfwSetWindowShouldClose(window.handle_, GLFW_TRUE);
            window.status_.should_close = true;
        } else if (cursor_x >= window_width - 2.0 * control_width) {
            if (glfwGetWindowAttrib(window.handle_, GLFW_MAXIMIZED) == GLFW_TRUE) {
                glfwRestoreWindow(window.handle_);
            } else {
                glfwMaximizeWindow(window.handle_);
            }
        } else if (cursor_x >= window_width - 3.0 * control_width) {
            glfwIconifyWindow(window.handle_);
        } else {
            window.interaction_.mode = WindowInteractionMode::Dragging;
            window.interaction_.drag_start_window_x = window_x;
            window.interaction_.drag_start_window_y = window_y;
            window.interaction_.drag_start_cursor_x = static_cast<double>(window_x) + cursor_x;
            window.interaction_.drag_start_cursor_y = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (!left_mouse_is_down) {
        window.interaction_.mode = WindowInteractionMode::Idle;
        window.interaction_.resize_edges = 0;
    } else if (window.interaction_.mode == WindowInteractionMode::Resizing) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        const int delta_x = static_cast<int>(
            cursor_screen_x - window.interaction_.drag_start_cursor_x);
        const int delta_y = static_cast<int>(
            cursor_screen_y - window.interaction_.drag_start_cursor_y);

        int width = window.interaction_.resize_start_width;
        int height = window.interaction_.resize_start_height;
        int x = window.interaction_.drag_start_window_x;
        int y = window.interaction_.drag_start_window_y;
        if ((window.interaction_.resize_edges & kResizeRight) != 0) width += delta_x;
        if ((window.interaction_.resize_edges & kResizeLeft) != 0) {
            width -= delta_x;
            x += delta_x;
        }
        if ((window.interaction_.resize_edges & kResizeBottom) != 0) height += delta_y;
        if ((window.interaction_.resize_edges & kResizeTop) != 0) {
            height -= delta_y;
            y += delta_y;
        }

        const int minimum_width = static_cast<int>(
            std::lround(320.0f * GetWindowScaleX(window.handle_)));
        const int minimum_height = static_cast<int>(
            std::lround(200.0f * GetWindowScaleY(window.handle_)));
        if (width < minimum_width) {
            if ((window.interaction_.resize_edges & kResizeLeft) != 0) {
                x -= minimum_width - width;
            }
            width = minimum_width;
        }
        if (height < minimum_height) {
            if ((window.interaction_.resize_edges & kResizeTop) != 0) {
                y -= minimum_height - height;
            }
            height = minimum_height;
        }
        glfwSetWindowPos(window.handle_, x, y);
        glfwSetWindowSize(window.handle_, width, height);
    } else if (window.interaction_.mode == WindowInteractionMode::Dragging) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        glfwSetWindowPos(
            window.handle_,
            window.interaction_.drag_start_window_x
                + static_cast<int>(cursor_screen_x - window.interaction_.drag_start_cursor_x),
            window.interaction_.drag_start_window_y
                + static_cast<int>(cursor_screen_y - window.interaction_.drag_start_cursor_y)
        );
    }

    window.interaction_.left_mouse_down = left_mouse_is_down;
}

