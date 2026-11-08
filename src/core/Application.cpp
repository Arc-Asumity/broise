// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/core/Application.cpp
// Application lifecycle and main loop.

#include <utility>

#include <glad/gl.h>

#include <self/core/Application.hpp>

Application::Application(
    WindowManager windows, Renderer renderer,
    const std::filesystem::path& installed_directory)
    : modules_(installed_directory)
    , windows_(std::move(windows))
    , renderer_(std::move(renderer))
{}

Application& Application::operator=(Application&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    renderer_ = std::move(other.renderer_);
    windows_ = std::move(other.windows_);
    modules_ = std::move(other.modules_);
    widgets_ = std::move(other.widgets_);
    return *this;
}

std::expected<Application, WindowError> Application::Create(
    const char* title, const std::filesystem::path& installed_directory)
{
    auto style = Style::Load(installed_directory / "config" / "style.toml");
    if (!style) {
        return std::unexpected(WindowError::StyleConfigurationFailed);
    }

    WindowManager windows;
    auto window = windows.CreateWindow(title);
    if (!window) {
        return std::unexpected(window.error());
    }

    auto renderer = Renderer::Create(
        window->get(), std::move(*style),
        installed_directory / "assets" / "icons");
    if (!renderer) {
        return std::unexpected(renderer.error());
    }

    return Application(std::move(windows), std::move(*renderer),
                       installed_directory);
}

const ModuleLoader& Application::GetModuleManager() const noexcept
{
    return modules_;
}

ModuleLoader& Application::GetModuleManager() noexcept
{
    return modules_;
}

void Application::Run()
{
    while (!renderer_.ShouldClose()) {
        windows_.PollEvents();
        renderer_.UpdateWindowInput();
        glClear(GL_COLOR_BUFFER_BIT);
        renderer_.RenderTitleBar();
        renderer_.SwapBuffers();
    }
}
