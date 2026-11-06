// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/TitleBarPart.hpp
// Header for the title bar Part.

#pragma once

#include <self/parts/ButtonPart.hpp>
#include <self/parts/IPart.hpp>
#include <self/parts/RectanglePart.hpp>
#include <self/parts/TextPart.hpp>

class TitleBarPart : public IPart
{
public:
    void Render(const PartRenderContext& context) override;
    void Update() override;

private:
    RectanglePart background_;
    ButtonPart minimize_button_;
    ButtonPart close_button_;
    RectanglePart minimize_icon_;
    RectanglePart close_icon_forward_;
    RectanglePart close_icon_backward_;
    TextPart title_;
};
