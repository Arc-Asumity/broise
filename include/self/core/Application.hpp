// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Application.hpp
// Header for application.

#pragma once

#include <expected>
#include <filesystem>

#include <self/core/Renderer.hpp>
#include <self/core/ModuleLoader.hpp>
#include <self/core/WidgetManager.hpp>

class Application
{
public:
    static std::expected<Application, WindowError>
    Create(const char* title, std::filesystem::path root_directory);

    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    [[nodiscard]] const ModuleLoader& GetModuleManager() const noexcept;
    [[nodiscard]] ModuleLoader& GetModuleManager() noexcept;

    void Run();

private:
    Renderer renderer_;
    ModuleLoader modules_;
    WidgetManager widgets_;

};

