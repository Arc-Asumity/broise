// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/widgets/IWidget.hpp
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

        [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
        virtual void Update() = 0;

        // Returns the root of this Widget's UI Part hierarchy. The Widget
        // retains ownership of its Parts; the returned reference is non-owning.
        [[nodiscard]] virtual ::IPart& GetRootPart() noexcept = 0;
    };
}
