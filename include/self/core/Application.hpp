// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Application.hpp
// Header for application.


#pragma once

#include <self/platform/WindowManager.hpp>
#include <self/modules/ModuleManager.hpp>
#include <self/assets/Icon.hpp>

class Application
{
public:
    explicit Application(std::string_view title, const std::filesystem::path& installed_directory);

    void Run();

private:
    WindowManager windows_;
    ModuleManager modules_;
    IconManager icons_;

    std::vector<IconImage> window_icon_;

    static void HandleError(WindowManagerError error);
};
