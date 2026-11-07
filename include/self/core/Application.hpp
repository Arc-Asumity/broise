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
#include <string_view>
#include <vector>

#include <self/core/Window.hpp>
#include <self/ui/Renderer.hpp>
#include <self/ui/Style.hpp>
#include <self/modules/ModuleLoader.hpp>
#include <self/parts/RootPart.hpp>
#include <self/modules/IWidget.hpp>

class Application
{
public:
    static std::expected<Application, WindowError>
    Create(const char* title, Style style, std::filesystem::path module_directory);

    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) noexcept = default;
    Application& operator=(Application&&) noexcept = default;

    [[nodiscard]] const Broise::ModuleLoader& GetModuleManager() const noexcept;
    [[nodiscard]] Broise::ModuleLoader& GetModuleManager() noexcept;

    [[nodiscard]] Broise::IWidget*
    AddWidget(std::string_view module_id, std::string_view widget_id);

    void AddWidget(Broise::IWidget& widget);
    void RemoveWidget(Broise::IWidget& widget) noexcept;
    void Run();

private:
    explicit Application(
        Window window,
        Style style,
        std::filesystem::path module_directory
    );

    Window window_;
    Renderer render_;
    Style style_;
    Broise::ModuleLoader module_manager_;
    RootPart root_;
    std::vector<Broise::IWidget*> widgets_;

};

