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
    Renderer renderer, const std::filesystem::path& installed_directory)
    : renderer_(std::move(renderer))
    , modules_(installed_directory)
{}

std::expected<Application, WindowError> Application::Create(
    const char* title, const std::filesystem::path& installed_directory)
{
    auto style = Style::Load(installed_directory / "config" / "style.toml");
    if (!style) {
        return std::unexpected(WindowError::StyleConfigurationFailed);
    }

    auto renderer = Renderer::Create(
        title, std::move(*style), installed_directory / "assets" / "icons");
    if (!renderer) {
        return std::unexpected(renderer.error());
    }

    return Application(std::move(*renderer), installed_directory);
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
        renderer_.PollEvents();
        renderer_.UpdateWindowInput();
        glClear(GL_COLOR_BUFFER_BIT);
        renderer_.RenderTitleBar();
        renderer_.SwapBuffers();
    }
}
