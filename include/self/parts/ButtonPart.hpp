// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/ButtonPart.hpp
// Header for a button Part.

#pragma once

#include <../ui/Rect.hpp>
#include <self/parts/IPart.hpp>
#include <self/parts/RectanglePart.hpp>

class ButtonPart : public IPart
{
public:
    void SetBounds(Rect bounds);
    void SetColor(Color color);

    void Render(const PartRenderContext& context) override;
    void Update() override;

private:
    RectanglePart background_;
    Rect bounds_{};
    Color color_{};
};
