// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/core/Application.cpp
// Application lifecycle and main loop.


#include <iostream>

#include <self/core/Application.hpp>

Application::Application(const std::string_view title, const std::filesystem::path& installed_directory)
    : modules_(installed_directory / "mods")
{
    windows_ = WindowManager();
    if (auto result = windows_.Initialize(title); result)
        HandleError(result.error());
}

void Application::HandleError(WindowManagerError error)
{
    std::cout << "Error code: " << static_cast<int>(error);
    exit(1);
}

