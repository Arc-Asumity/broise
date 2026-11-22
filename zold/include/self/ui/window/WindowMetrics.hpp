// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/window/WindowMetrics.hpp
// Logical and framebuffer dimensions and content scale for a window.

#pragma once

struct WindowMetrics
{
    int framebuffer_width{};
    int framebuffer_height{};
    int logical_width{};
    int logical_height{};
    float content_scale_x{1.0f};
    float content_scale_y{1.0f};
};
