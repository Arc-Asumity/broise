// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Default native C++ Module entry point and Widget registry.

#include <array>
#include <span>
#include <string_view>

#include <internal/modules/default/ClockWidget.hpp>
#include <self/modules/IModule.hpp>

namespace
{
    class DefaultModule final : public Broise::IModule
    {
    public:
        [[nodiscard]] Broise::ModuleInfo GetInfo() const override
        {
            return {
                .id = "broise.default",
                .name = "Broise Default Module",
                .version = "1.0.0",
                .api_version = Broise::kModuleApiVersion,
                .pointer_size = sizeof(void*),
                .abi_tag = Broise::kModuleAbiTag,
            };
        }

        [[nodiscard]] std::span<Broise::IWidgetEntry* const>
        GetWidgetEntries() const noexcept override
        {
            return entries_;
        }

        bool Initialize(const Broise::ModuleContext&) override
        {
            return true;
        }

        bool Shutdown() noexcept override
        {
            return true;
        }

        void Destroy() noexcept override
        {
            delete this;
        }

    private:
        class ClockEntry final : public Broise::IWidgetEntry
        {
        public:
            [[nodiscard]] std::string_view GetId() const noexcept override
            {
                return "clock";
            }

            [[nodiscard]] std::string_view GetName() const noexcept override
            {
                return "Clock";
            }

            [[nodiscard]] std::string_view GetDescription() const noexcept override
            {
                return "Displays the local time.";
            }

            [[nodiscard]] Broise::IWidget* Create() override
            {
                return new ClockWidget();
            }

            void Destroy(Broise::IWidget* widget) noexcept override
            {
                delete widget;
            }
        } clock_entry_;

        std::array<Broise::IWidgetEntry*, 1> entries_{&clock_entry_};
    };
}

namespace Broise
{
    BROISE_MODULE_EXPORT IModule* CreateModuleV1()
    {
        return new DefaultModule();
    }
}
