// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/ModuleManager.hpp
// Runtime API for discovering and managing native C++ Modules.

#pragma once

#include <filesystem>

class ModuleManager
{
public:
    explicit ModuleManager(const std::filesystem::path& module_path);
};

