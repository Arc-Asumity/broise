// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/ModuleManager.hpp
// Runtime API for discovering and managing native C++ Modules.

#pragma once

#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <self/modules/IModule.hpp>

namespace Broise
{
    enum class ModuleDiagnosticCode
    {
        DirectoryScanFailed,
        LibraryLoadFailed,
        EntryPointNotFound,
        FactoryFailed,
        InvalidMetadata,
        IncompatibleApi,
        IncompatibleAbi,
        DuplicateModuleId,
        DuplicateWidgetId,
        InitializationFailed,
        WidgetCreationFailed,
        ShutdownFailed,
    };

    struct ModuleDiagnostic
    {
        std::filesystem::path path;
        ModuleDiagnosticCode code;
        std::string message;
    };

    struct LoadedWidgetInfo
    {
        std::string id;
        std::string name;
        std::string description;
    };

    struct LoadedModuleInfo
    {
        std::filesystem::path path;
        std::string id;
        std::string name;
        std::string version;
        ApiVersion api_version;
        std::vector<LoadedWidgetInfo> widgets;
    };

    class ModuleManager
    {
    public:
        ModuleManager();
        ~ModuleManager();

        ModuleManager(const ModuleManager&) = delete;
        ModuleManager& operator=(const ModuleManager&) = delete;

        ModuleManager(ModuleManager&&) noexcept;
        ModuleManager& operator=(ModuleManager&&) noexcept;

        // Scans configured directories recursively. A failed candidate is
        // recorded and does not prevent other candidates from loading.
        void DiscoverAndLoad(std::span<const std::filesystem::path> directories);

        // Returns a non-owning pointer. The ModuleManager owns the instance
        // and destroys it before shutting down its providing Module.
        [[nodiscard]] IWidget* CreateWidget(std::string_view module_id,
                                            std::string_view widget_id);
        void DestroyWidget(IWidget& widget) noexcept;

        [[nodiscard]] std::span<const LoadedModuleInfo>
        GetLoadedModules() const noexcept;
        [[nodiscard]] std::span<const ModuleDiagnostic>
        GetDiagnostics() const noexcept;

        // Shuts down Modules and unloads those that report a successful
        // shutdown. Unsafe-to-unload Modules remain pinned for process life.
        void Shutdown() noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
}
