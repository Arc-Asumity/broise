// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/parts/TextPart.cpp
// Text Part implementation.

#include <utility>

#include <../../include/self/ui/Renderer.hpp>
#include <self/parts/TextPart.hpp>

void TextPart::SetText(const std::string_view text)
{
    text_ = text;
}

void TextPart::SetFontStyle(FontStyle font)
{
    font_ = std::move(font);
}

void TextPart::SetBounds(const Rect bounds)
{
    bounds_ = bounds;
}

void TextPart::SetColor(const Color color)
{
    color_ = color;
}

void TextPart::Render(const PartRenderContext& context)
{
    if (context.logical_width <= 0.0f || context.logical_height <= 0.0f
        || context.framebuffer_width <= 0.0f
        || context.framebuffer_height <= 0.0f) {
        return;
    }

    const float scale_x = context.framebuffer_width / context.logical_width;
    const float scale_y = context.framebuffer_height / context.logical_height;
    FontStyle framebuffer_font = font_;
    framebuffer_font.size *= scale_y;

    context.renderer.RenderText(
        text_, framebuffer_font, bounds_.x * scale_x, bounds_.y * scale_y,
        context.framebuffer_width, context.framebuffer_height, color_);
}

void TextPart::Update() {}
