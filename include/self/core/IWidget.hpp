// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/IWidget.hpp
// Interface for Widget.

#pragma once

class IWidget
{
public:
    virtual ~IWidget() = default;

    virtual void Render() = 0;
    virtual void Update() = 0;
};
