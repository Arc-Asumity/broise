// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/TextPart.hpp
// Header for a text Part.

#pragma once

#include <string>
#include <string_view>

#include <self/core/Rect.hpp>
#include <self/core/Style.hpp>
#include <self/parts/IPart.hpp>

class TextPart : public IPart
{
public:
    void SetText(std::string_view text);
    void SetFontStyle(FontStyle font);
    void SetBounds(Rect bounds);
    void SetColor(Color color);

    void Render(const PartRenderContext& context) override;
    void Update() override;

private:
    std::string text_;
    FontStyle font_;
    Rect bounds_{};
    Color color_{};
};
