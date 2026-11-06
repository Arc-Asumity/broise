// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/parts/ButtonPart.cpp
// Button Part implementation.

#include <self/parts/ButtonPart.hpp>

void ButtonPart::SetBounds(const Rect bounds)
{
    bounds_ = bounds;
}

void ButtonPart::SetColor(const Color color)
{
    color_ = color;
}

void ButtonPart::Render(const PartRenderContext& context)
{
    background_.SetBounds(bounds_);
    background_.SetColor(color_);
    background_.Render(context);
}

void ButtonPart::Update()
{
    background_.Update();
}
