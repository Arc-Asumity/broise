// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/module/IWidgetEntry.hpp
// A Module-owned registration entry for one Widget type.

#pragma once

#include <self/modules/IWidget.hpp>

namespace Broise
{
    class IWidgetEntry
    {
    public:
        virtual ~IWidgetEntry() = default;

        [[nodiscard]] virtual std::string_view GetId() const noexcept = 0;

        virtual IWidget* Create() = 0;

        virtual void Destroy(IWidget* widget) noexcept = 0;
    };
}
