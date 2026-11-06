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
Application::Create(const char* title, IPart& root)
{
    auto window = Window::Create(title);

    if (!window) {
        return std::unexpected(window.error());
    }

    return Application{
        std::move(*window),
        root,
    };
}

Application::Application(Window window, IPart& root)
    : window_(std::move(window))
    , root_(root)
    , render_(Renderer())
{
    render_.Init();
}

void Application::Run()
{
    glClearColor(0, 0, 0, 1);


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
                  Color{0.12f, 0.13f, 0.15f, 1.0f});

        const float glyph_pixel_width = title_bar_pixels / 10.0f;
        const float glyph_pixel_height = title_bar_pixels / 10.0f;
        const float glyph_margin = title_bar_pixels * 0.6f;
        const float glyph_top = (title_bar_pixels - 5.0f * glyph_pixel_height) / 2.0f;
        for (int glyph = 0; glyph < 6; ++glyph) {
            for (int row = 0; row < 5; ++row) {
                for (int column = 0; column < 3; ++column) {
                    constexpr unsigned char title_glyphs[6][5] = {
                        {0b110, 0b101, 0b110, 0b101, 0b110}, // B
                        {0b000, 0b110, 0b100, 0b100, 0b100}, // r
                        {0b000, 0b110, 0b101, 0b101, 0b110}, // o
                        {0b010, 0b000, 0b010, 0b010, 0b010}, // i
                        {0b000, 0b011, 0b100, 0b010, 0b110}, // s
                        {0b010, 0b100, 0b110, 0b100, 0b010}, // e
                    };
                    if ((title_glyphs[glyph][row] & (1 << (2 - column))) == 0) {
                        continue;
                    }
                    const float left = (glyph_margin + static_cast<float>(glyph) * 4.0f * glyph_pixel_width
                                        + static_cast<float>(column) * glyph_pixel_width) / window_width;
                    const float top = (glyph_top + static_cast<float>(row) * glyph_pixel_height) / window_height;
                    draw_rect(left, top,
                              left + glyph_pixel_width / window_width,
                              top + glyph_pixel_height / window_height,
                              Color{0.88f, 0.89f, 0.92f, 1.0f});
                }
            }
        }

        draw_rect(1.0f - 2.0f * control_width / window_width, 0.0f,
                  1.0f - control_width / window_width, title_bar_height,
                  Color{0.16f, 0.17f, 0.19f, 1.0f});
        draw_rect(1.0f - control_width / window_width, 0.0f,
                  1.0f, title_bar_height,
                  Color{0.65f, 0.12f, 0.14f, 1.0f});

        const float icon_half_width = title_bar_pixels * 0.14f / window_width;
        const float icon_half_height = title_bar_pixels * 0.035f / window_height;
        const float icon_center_y = title_bar_height / 2.0f;
        const float minimize_center_x = 1.0f - 1.5f * control_width / window_width;
        const float close_center_x = 1.0f - 0.5f * control_width / window_width;
        constexpr Color icon_color{0.92f, 0.93f, 0.95f, 1.0f};
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
