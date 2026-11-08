// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /example/TestModule.cpp
// Test module for Broise.


#include <iostream>
#include <span>

#include <self/modules/IModule.hpp>
#include <self/modules/IWidgetEntry.hpp>

namespace
{
    class TestModule final : public Broise::IModule
    {
    public:
        [[nodiscard]] std::span<Broise::IWidgetEntry* const>
        GetEntries() const noexcept override
        {
            return {};
        }

        bool Initialize() override
        {
            std::cout << "[TestModule] Initialized\n";
            return true;
        }

        bool Shutdown() noexcept override
        {
            std::cout << "[TestModule] Shutdown\n";
            return true;
        }
    };
}

namespace Broise
{
    extern "C" BROISE_MODULE_EXPORT
    IModule* CreateModuleV1() noexcept
    {
        return new (std::nothrow) TestModule();
    }

    extern "C" BROISE_MODULE_EXPORT
    void DestroyModuleV1(const IModule* module) noexcept
    {
        delete module;
    }

    extern "C" BROISE_MODULE_EXPORT
    const char* GetModuleAbiTagV1() noexcept
    {
        return BROISE_MODULE_ABI_TAG;
    }
}
