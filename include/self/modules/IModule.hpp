// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/IModule.hpp
// Public interface for an application or feature Module.

#pragma once

#include <self/modules/IWidgetEntry.hpp>


class IModule
{
public:
    virtual ~IModule() = default;

    [[nodiscard]] virtual std::vector<IWidgetEntry*>
    GetWidgetEntries() noexcept = 0;

    virtual bool Initialize() = 0;

    virtual bool Shutdown() noexcept = 0;
};
