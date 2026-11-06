// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Application.cpp
// Application for software.


#include <iostream>
#include <glad/gl.h>

#include <self/core/Application.hpp>

std::expected<Application, WindowError>
Application::Create(const char* title, Style style)
{
    auto window = Window::Create(title);

    if (!window) {
        return std::unexpected(window.error());
    }

    return Application{
        std::move(*window),
        std::move(style),
    };
}

Application::Application(Window window, Style style)
    : window_(std::move(window))
    , render_(Renderer())
    , style_(std::move(style))
    , root_()
{
    render_.Init();
}

void Application::Run()
{
    glClearColor(style_.background_color.r, style_.background_color.g,
                 style_.background_color.b, style_.background_color.a);


    while (!window_.ShouldClose()) {
        Window::PollEvents();
        window_.UpdateTitleBarInput();
        root_.Update();

        glClear(GL_COLOR_BUFFER_BIT);

        root_.Render();

        const auto window_width = static_cast<float>(window_.GetLogicalWidth());
        const auto window_height = static_cast<float>(window_.GetLogicalHeight());
        const auto title_bar_pixels = static_cast<float>(window_.GetTitleBarHeight());
        const float title_bar_height = title_bar_pixels / window_height;
        const auto control_width = static_cast<float>(window_.GetTitleBarControlWidth());
        const auto draw_rect = [this](const float left, const float top,
                                      const float right, const float bottom,
                                      const Color& color) {
            render_.RenderPolygon({
                {left, top}, {right, top}, {right, bottom}, {left, bottom}
            }, color);
        };

        draw_rect(0.0f, 0.0f, 1.0f, title_bar_height,
                  style_.title_bar_color);

        const FontStyle& title_font = style_.fonts.at("title");
        const float title_font_size = title_font.size * title_bar_pixels / 32.0f;
        const float title_y = (title_bar_pixels - title_font_size) / 2.0f;
        const FontStyle scaled_title_font{
            .file = title_font.file,
            .size = title_font_size,
        };
        render_.RenderText("Broise", scaled_title_font, title_bar_pixels * 0.6f,
                           title_y, window_width, window_height,
                           style_.title_bar_icon_color);

        draw_rect(1.0f - 2.0f * control_width / window_width, 0.0f,
                  1.0f - control_width / window_width, title_bar_height,
                  style_.title_bar_button_color);
        draw_rect(1.0f - control_width / window_width, 0.0f,
                  1.0f, title_bar_height,
                  style_.close_button_color);

        const float icon_half_width = title_bar_pixels * 0.14f / window_width;
        const float icon_half_height = title_bar_pixels * 0.035f / window_height;
        const float icon_center_y = title_bar_height / 2.0f;
        const float minimize_center_x = 1.0f - 1.5f * control_width / window_width;
        const float close_center_x = 1.0f - 0.5f * control_width / window_width;
        const Color& icon_color = style_.title_bar_icon_color;
        draw_rect(minimize_center_x - icon_half_width,
                  icon_center_y - icon_half_height,
                  minimize_center_x + icon_half_width,
                  icon_center_y + icon_half_height,
                  icon_color);
        const float cross_radius = title_bar_pixels * 0.14f;
        const float cross_thickness = title_bar_pixels * 0.035f;
        render_.RenderPolygon({
            {close_center_x - cross_radius / window_width,
             icon_center_y - (cross_radius - cross_thickness) / window_height},
            {close_center_x - (cross_radius - cross_thickness) / window_width,
             icon_center_y - cross_radius / window_height},
            {close_center_x + cross_radius / window_width,
             icon_center_y + (cross_radius - cross_thickness) / window_height},
            {close_center_x + (cross_radius - cross_thickness) / window_width,
             icon_center_y + cross_radius / window_height}
        }, icon_color);
        render_.RenderPolygon({
            {close_center_x - cross_radius / window_width,
             icon_center_y + (cross_radius - cross_thickness) / window_height},
            {close_center_x - (cross_radius - cross_thickness) / window_width,
             icon_center_y + cross_radius / window_height},
            {close_center_x + cross_radius / window_width,
             icon_center_y - (cross_radius - cross_thickness) / window_height},
            {close_center_x + (cross_radius - cross_thickness) / window_width,
             icon_center_y - cross_radius / window_height}
        }, icon_color);

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
