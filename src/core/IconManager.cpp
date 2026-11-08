// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/IconManager.cpp
// Loads and caches SVG source for Lucide icons.

#include <fstream>
#include <iterator>
#include <string>
#include <utility>

#include <self/core/IconManager.hpp>

namespace
{
    bool IsValidIconName(const std::string_view name)
    {
        if (name.empty()) {
            return false;
        }
        for (const char character : name) {
            if ((character < 'a' || character > 'z')
                && (character < '0' || character > '9') && character != '-') {
                return false;
            }
        }
        return true;
    }
}

std::expected<Icon, std::string> IconManager::GetIcon(
    const std::string_view name)
{
    const std::string key{name};
    if (const auto cached = icon_cache_.find(key); cached != icon_cache_.end()) {
        return cached->second;
    }

    auto svg = GetSvg(name);
    if (!svg) {
        return std::unexpected(svg.error());
    }

    return icon_cache_.emplace(key, Icon{std::move(*svg)}).first->second;
}

IconManager::IconManager(std::filesystem::path icon_directory)
    : icon_directory_(std::move(icon_directory))
{}

std::expected<std::string, std::string> IconManager::GetSvg(
    const std::string_view name)
{
    if (!IsValidIconName(name)) {
        return std::unexpected("Invalid Lucide icon name");
    }

    const std::string key{name};
    if (const auto cached = svg_cache_.find(key); cached != svg_cache_.end()) {
        return cached->second;
    }

    std::ifstream file(icon_directory_ / (key + ".svg"), std::ios::binary);
    if (!file) {
        return std::unexpected("Lucide icon not found: " + key);
    }

    std::string svg{std::istreambuf_iterator<char>{file},
                    std::istreambuf_iterator<char>{}};
    if (file.bad()) {
        return std::unexpected("Failed to read Lucide icon: " + key);
    }

    return svg_cache_.emplace(key, std::move(svg)).first->second;
}
