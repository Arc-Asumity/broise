// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/window/WindowInteraction.hpp
// Current pointer interaction and drag or resize state for a window.

#pragma once

enum class WindowInteractionMode
{
    Idle,
    Dragging,
    Resizing,
};

struct WindowInteraction
{
    WindowInteractionMode mode{WindowInteractionMode::Idle};
    bool left_mouse_down{};
    int resize_edges{};
    int drag_start_window_x{};
    int drag_start_window_y{};
    double drag_start_cursor_x{};
    double drag_start_cursor_y{};
    int resize_start_width{};
    int resize_start_height{};
};
