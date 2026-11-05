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
#include <self/parts/IPart.hpp>

class Application
{
public:
    static std::expected<Application, WindowError>
    Create(const char* title, IPart& root);

    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) noexcept = default;
    Application& operator=(Application&&) noexcept = default;

    void Run();

private:
    explicit Application(Window window, IPart& root);

    Window window_;
    IPart& root_;
    Renderer render_;

};

