// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// A Module-owned registration entry for one Widget type.

#pragma once

#include <string_view>

#include <IWidget.hpp>

namespace Broise
{
    class IWidgetEntry
    {
    public:
        virtual ~IWidgetEntry() = default;

        [[nodiscard]] virtual std::string_view GetId() const noexcept = 0;
        [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
        [[nodiscard]] virtual std::string_view GetDescription() const noexcept = 0;
        [[nodiscard]] virtual IWidget* Create() = 0;
        virtual void Destroy(IWidget* widget) noexcept = 0;
    };
}
