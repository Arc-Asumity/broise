// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/IconManager.hpp
// Loads and caches SVG source for Lucide icons.

#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>


struct Icon
{
    std::string svg;
};

class IconManager
{
public:
    explicit IconManager(std::filesystem::path icon_directory);

    [[nodiscard]] std::expected<std::string, std::string>
    GetSvg(std::string_view name);
    [[nodiscard]] std::expected<Icon, std::string>
    GetIcon(std::string_view name);

private:
    std::filesystem::path icon_directory_;
    std::unordered_map<std::string, std::string> svg_cache_;
    std::unordered_map<std::string, Icon> icon_cache_;
};
