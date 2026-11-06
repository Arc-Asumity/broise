// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Application.hpp
// Header for application.

#pragma once

#include <expected>

#include <self/core/Window.hpp>
#include <self/core/Renderer.hpp>
#include <self/core/Style.hpp>
#include <self/parts/RootPart.hpp>

class Application
{
public:
    static std::expected<Application, WindowError>
    Create(const char* title, Style style);

    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) noexcept = default;
    Application& operator=(Application&&) noexcept = default;

    void Run();

private:
    explicit Application(Window window, Style style);

    Window window_;
    Renderer render_;
    Style style_;
    RootPart root_;

};

