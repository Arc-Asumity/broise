// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/Font.hpp
// Font asset metadata.

#pragma once

#include <filesystem>

struct Font
{
    std::filesystem::path file;
    float size{};
};
