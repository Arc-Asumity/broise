// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Renderer.hpp
// Header file for render of UI.

#pragma once

#include <self/core/Base.hpp>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    void Init();
    void RenderPolygon(const PointSet& points, const Color& color) const;

private:
    unsigned int program_{};
    unsigned int vao_{};
    unsigned int vbo_{};
};
