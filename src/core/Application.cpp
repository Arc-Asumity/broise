// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Application.cpp
// Application for software.


#include <algorithm>
#include <iostream>
#include <utility>

#include <glad/gl.h>

#include <self/core/Application.hpp>

std::expected<Application, WindowError>
Application::Create(
    const char* title,
    Style style,
    std::filesystem::path module_directory)
{
    auto window = Window::Create(title);

    if (!window) {
        return std::unexpected(window.error());
    }

    Application application{
        std::move(*window),
        std::move(style),
        std::move(module_directory),
    };
    application.module_manager_.DiscoverAndLoad(
        application.module_directory_);
    return std::move(application);
}

Application::Application(
    Window window, Style style,
    std::vector<std::filesystem::path> module_directory)
    : window_(std::move(window))
    , render_(Renderer())
    , style_(std::move(style))
    , module_directory_(std::move(module_directory))
    , root_()
{
    render_.Init();
}

const Broise::ModuleLoader& Application::GetModuleManager() const noexcept
{
    return module_manager_;
}

Broise::ModuleLoader& Application::GetModuleManager() noexcept
{
    return module_manager_;
}

Broise::IWidget* Application::AddWidget(
    const std::string_view module_id, const std::string_view widget_id)
{
    Broise::IWidget* widget = module_manager_.CreateWidget(module_id, widget_id);
    if (widget != nullptr) {
        AddWidget(*widget);
    }
    return widget;
}

void Application::AddWidget(Broise::IWidget& widget)
{
    if (std::find(widgets_.begin(), widgets_.end(), &widget) == widgets_.end()) {
        widgets_.push_back(&widget);
    }
}

void Application::RemoveWidget(Broise::IWidget& widget) noexcept
{
    std::erase(widgets_, &widget);
}

void Application::Run()
{
    for (const Broise::ModuleDiagnostic& diagnostic :
         module_manager_.GetDiagnostics()) {
        std::cerr << "Module load error ["
                  << static_cast<int>(diagnostic.code) << "] "
                  << diagnostic.path.string() << ": "
                  << diagnostic.message << '\n';
    }

    glClearColor(style_.background_color.r, style_.background_color.g,
                 style_.background_color.b, style_.background_color.a);


    while (!window_.ShouldClose()) {
        Window::PollEvents();
        window_.UpdateTitleBarInput();
        root_.Update();
        for (Broise::IWidget* widget : widgets_) {
            widget->Update();
        }

        glClear(GL_COLOR_BUFFER_BIT);

        const PartRenderContext render_context{
            .renderer = render_,
            .style = style_,
            .logical_width = static_cast<float>(window_.GetLogicalWidth()),
            .logical_height = static_cast<float>(window_.GetLogicalHeight()),
            .framebuffer_width = static_cast<float>(window_.GetWidth()),
            .framebuffer_height = static_cast<float>(window_.GetHeight()),
            .title_bar_height = static_cast<float>(window_.GetTitleBarHeight()),
            .title_bar_control_width = static_cast<float>(
                window_.GetTitleBarControlWidth()),
        };
        root_.Render(render_context);
        for (Broise::IWidget* widget : widgets_) {
            widget->GetRootPart().Render(render_context);
        }

        window_.SwapBuffers();
    }
}

/***
Widget& Application::GetRoot()
{
    return root_;
}

Widget* Application::GetPressedWidget(const Point& point)
{
    return root_.GetPressedWidget(point);
}
***/
