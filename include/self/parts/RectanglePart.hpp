// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/RectanglePart.hpp
// Header for a rectangle Part.

#pragma once

#include <self/core/Rect.hpp>
#include <self/parts/IPart.hpp>

class RectanglePart : public IPart
{
public:
    void SetBounds(Rect bounds);
    void SetColor(Color color);
    void SetRotation(float radians);

    void Render(const PartRenderContext& context) override;
    void Update() override;

private:
    Rect bounds_{};
    Color color_{};
    float rotation_{};
};
