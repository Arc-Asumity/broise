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

#include <self/modules/IModule.hpp>

class ModuleLoader
{
public:
    explicit ModuleLoader(std::filesystem::path module_path);
    ~ModuleLoader();

    ModuleLoader(const ModuleLoader&) = delete;
    ModuleLoader& operator=(const ModuleLoader&) = delete;

    ModuleLoader(ModuleLoader&&) noexcept;
    ModuleLoader& operator=(ModuleLoader&&) noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

};

