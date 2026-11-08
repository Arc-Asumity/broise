// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/IPart.hpp
// Interface for a Runtime UI Part.

#pragma once

class Renderer;
struct Style;

struct PartRenderContext
{
    Renderer& renderer;
    const Style& style;
    float logical_width;
    float logical_height;
    float framebuffer_width;
    float framebuffer_height;
    float title_bar_height;
    float title_bar_control_width;
};

class IPart
{
public:
    virtual ~IPart() = default;

    virtual void Render(const PartRenderContext& context) = 0;
    virtual void Update() = 0;
};
