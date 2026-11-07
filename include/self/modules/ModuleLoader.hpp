// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/ModuleLoader.hpp
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

    class ModuleLoader
    {
    public:
        ModuleLoader();
        ~ModuleLoader();

        ModuleLoader(const ModuleLoader&) = delete;
        ModuleLoader& operator=(const ModuleLoader&) = delete;

        ModuleLoader(ModuleLoader&&) noexcept;
        ModuleLoader& operator=(ModuleLoader&&) noexcept;

        void DiscoverAndLoad(std::span<const std::filesystem::path> directories);

        [[nodiscard]] IWidget*
        CreateWidget(std::string_view module_id, std::string_view widget_id);
        void DestroyWidget(IWidget& widget) noexcept;

        [[nodiscard]] std::span<const LoadedModuleInfo>
        GetLoadedModules() const noexcept;
        [[nodiscard]] std::span<const ModuleDiagnostic>
        GetDiagnostics() const noexcept;

        void Shutdown() noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
}
