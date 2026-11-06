// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Renderer.hpp
// Header file for render of UI.

#pragma once

#include <memory>
#include <string_view>

#include <self/core/Base.hpp>
#include <self/core/Style.hpp>

class FontRenderer;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    void Init();
    void RenderPolygon(const PointSet& points, const Color& color) const;
    void RenderText(std::string_view text, const FontStyle& font,
                    float x, float y, float width, float height,
                    const Color& color);

private:
    unsigned int program_{};
    unsigned int vao_{};
    unsigned int vbo_{};
    std::unique_ptr<FontRenderer> font_renderer_;
};
