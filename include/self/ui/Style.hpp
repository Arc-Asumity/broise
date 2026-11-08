// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/Style.hpp
// Style values loaded from a TOML configuration file.


#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>

#include <self/ui/Base.hpp>

struct FontStyle
{
    std::filesystem::path file;
    float size{};
};

struct StyleColors
{
    Color background{};
    Color bar{};
    Color button{};
    Color hover{};
    Color text{};
};

struct Style
{
    StyleColors colors;
    std::unordered_map<std::string, FontStyle> fonts;

    [[nodiscard]] static std::expected<Style, std::string>
    Load(const std::filesystem::path& path);
};
