// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/WindowManagerError.hpp
// Error for manages GLFW and owns application windows.


#pragma once

enum class WindowManagerError
{
    GlfwInitializationFailed = 1,
    PrimaryMonitorUnavailable = 2,
    VideoModeUnavailable = 3,
    GladInitializationFailed = 4,
    WindowCreationFailed = 5,
};
