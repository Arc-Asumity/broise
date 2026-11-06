// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/parts/RootPart.cpp
// Root Part.


#include <self/parts/RootPart.hpp>

RootPart::RootPart() = default;

void RootPart::Render(const PartRenderContext& context)
{
    title_bar_.Render(context);
}

void RootPart::Update()
{
}
