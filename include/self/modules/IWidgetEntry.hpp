// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// A Module-owned registration entry for one Widget type.

#pragma once

#include <self/modules/IWidget.hpp>

class IWidgetEntry
{
public:
    virtual ~IWidgetEntry() = default;

    [[nodiscard]] virtual IWidget* Create() = 0;
};

