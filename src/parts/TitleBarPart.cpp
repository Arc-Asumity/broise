// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/parts/TitleBarPart.cpp
// Title bar Part implementation.

#include <cmath>

#include <../../include/self/ui/Renderer.hpp>
#include <self/core/Style.hpp>
#include <self/parts/TitleBarPart.hpp>

void TitleBarPart::Render(const PartRenderContext& context)
{
    const Style& style = context.style;
    const float window_width = context.logical_width;
    const float window_height = context.logical_height;
    const float title_bar_pixels = context.title_bar_height;
    const float title_bar_height = title_bar_pixels / window_height;
    const float control_width = context.title_bar_control_width;

    background_.SetBounds({.x = 0.0f, .y = 0.0f, .width = 1.0f, .height = title_bar_height});
    background_.SetColor(style.title_bar_color);
    background_.Render(context);

    const auto& [file, size] = style.fonts.at("title");
    const float title_font_size = size * title_bar_pixels / 32.0f;
    const float title_y = (title_bar_pixels - title_font_size) / 2.0f
                          - title_font_size * 0.1f;
    const FontStyle scaled_title_font{
        .file = file,
        .size = title_font_size,
    };
    title_.SetText("Broise");
    title_.SetFontStyle(scaled_title_font);
    title_.SetBounds({.x = title_bar_pixels * 0.6f, .y = title_y,
                      .width = window_width, .height = window_height});
    title_.SetColor(style.title_bar_icon_color);
    title_.Render(context);

    minimize_button_.SetBounds({
        1.0f - 2.0f * control_width / window_width,
        0.0f,
        control_width / window_width,
        title_bar_height,
    });
    minimize_button_.SetColor(style.title_bar_button_color);
    minimize_button_.Render(context);

    close_button_.SetBounds({
        .x = 1.0f - control_width / window_width,
        .y = 0.0f,
        .width = control_width / window_width,
        .height = title_bar_height,
    });
    close_button_.SetColor(style.close_button_color);
    close_button_.Render(context);

    const float icon_half_width = title_bar_pixels * 0.14f / window_width;
    const float icon_half_height = title_bar_pixels * 0.035f / window_height;
    const float icon_center_y = title_bar_height / 2.0f;
    const float minimize_center_x =
        1.0f - 1.5f * control_width / window_width;
    const float close_center_x = 1.0f - 0.5f * control_width / window_width;
    const Color& icon_color = style.title_bar_icon_color;

    minimize_icon_.SetBounds({
        .x = minimize_center_x - icon_half_width,
        .y = icon_center_y - icon_half_height,
        .width = icon_half_width * 2.0f,
        .height = icon_half_height * 2.0f,
    });
    minimize_icon_.SetColor(icon_color);
    minimize_icon_.Render(context);

    const float cross_radius = title_bar_pixels * 0.14f;
    const float cross_thickness = title_bar_pixels * 0.035f;
    const float cross_length = 2.0f * std::sqrt(
        cross_radius * cross_radius
        + (cross_radius - cross_thickness)
              * (cross_radius - cross_thickness));
    const float cross_width = std::sqrt(2.0f) * cross_thickness;
    const float cross_angle = std::atan2(
        cross_radius - cross_thickness, cross_radius);
    const Rect cross_bounds{
        close_center_x - cross_length / (2.0f * window_width),
        icon_center_y - cross_width / (2.0f * window_height),
        cross_length / window_width,
        cross_width / window_height,
    };

    close_icon_forward_.SetBounds(cross_bounds);
    close_icon_forward_.SetColor(icon_color);
    close_icon_forward_.SetRotation(cross_angle);
    close_icon_forward_.Render(context);

    close_icon_backward_.SetBounds(cross_bounds);
    close_icon_backward_.SetColor(icon_color);
    close_icon_backward_.SetRotation(-cross_angle);
    close_icon_backward_.Render(context);
}

void TitleBarPart::Update() {}
