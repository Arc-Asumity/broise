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
    if (auto result = windows_.Initialize(title); !result)
        HandleError(result.error());


    // Test multi screen
    if (auto result = windows_.CreateWindow("Broise 2"); !result)
        HandleError(result.error());
}

void Application::Run()
{
    while (!windows_.ShouldClose())
    {
        if (auto result = windows_.Update(); !result)
            HandleError(result.error());
        if (auto result = windows_.Frame(); !result)
            HandleError(result.error());
        windows_.RemoveClosed();
    }
}

void Application::HandleError(WindowManagerError error)
{
    std::cout << "Error code: " << static_cast<int>(error);
    exit(1);
}

