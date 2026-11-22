// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/Monitor.cpp
// Monitor manager.


#include <expected>
#include <GLFW/glfw3.h>

#include <self/platform/Monitor.hpp>

std::expected<Monitor, WindowManagerError> QueryMonitor(GLFWmonitor* native_handle)
{
    Monitor monitor{};
    monitor.native_handle = native_handle;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor.native_handle);

    if (mode == nullptr)
        return std::unexpected(WindowManagerError::VideoModeUnavailable);

    monitor.height = mode->height;
    monitor.width = mode->width;
    monitor.refresh_rate = mode->refreshRate;

    glfwGetMonitorPos(monitor.native_handle, &monitor.position_x, &monitor.position_y);

    return monitor;
}
