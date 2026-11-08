// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/IModule.hpp
// Public interface for an application or feature Module.


#pragma once

#include <span>

#include <self/modules/Preprocess.hpp>
#include <self/modules/IWidgetEntry.hpp>

namespace Broise
{
    class IModule
    {
    public:
        virtual ~IModule() = default;

        [[nodiscard]] virtual std::span<IWidgetEntry* const>
        GetEntries() const noexcept = 0;

        virtual bool Initialize() = 0;

        virtual bool Shutdown() noexcept = 0;
    };

    extern "C" BROISE_MODULE_EXPORT
    IModule* CreateModule() noexcept;

    extern "C" BROISE_MODULE_EXPORT
    void DestroyModule(const IModule* module) noexcept;
}
