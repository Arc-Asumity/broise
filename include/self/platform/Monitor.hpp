// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/platform/Monitor.hpp
// Header for monitor.


#pragma once

#include <GLFW/glfw3.h>

#include <self/platform/WindowManager.hpp>

struct Monitor
{
    GLFWmonitor* native_handle;

    int width;
    int height;
    int refresh_rate;

    int position_x;
    int position_y;
};

std::expected<Monitor, WindowManagerError> QueryMonitor(GLFWmonitor* native_handle);
