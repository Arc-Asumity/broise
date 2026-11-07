// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/module/IWidget.hpp
// Public interface for an application-provided software Widget.

#pragma once

#include <string_view>

#include <self/parts/IPart.hpp>

namespace Broise
{
    class IWidget
    {
    public:
        virtual ~IWidget() = default;

        virtual void Update() = 0;
    };
}
