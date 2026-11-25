// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/assets/Icon.hpp
// Loads and caches SVG source for Lucide icons.


#pragma once

#include <expected>
#include <filesystem>
#include <vector>

constexpr std::array kWindowIconSize = {
    16,
    24,
    32,
    48,
    64,
    128,
    256
};

enum class IconManagerError
{
    NotFound = 0,
    ReadFailed = 1,
    LoadDocumentFailed = 2,
    RasterizeFailed = 3,
};

constexpr std::array<std::string_view, 4> kIconManagerErrorName = {
    "Icon Not Found",
    "Read Failed",
    "Load Document Failed",
    "Rasterize Failed"
};

struct IconImage {
    int width;
    int height;
    std::vector<unsigned char> pixels;
};

using WindowIconImages = std::array<IconImage, kWindowIconSize.size()>;

class IconManager
{
public:
    explicit IconManager(const std::filesystem::path& icons_path);

    [[nodiscard]] std::expected<WindowIconImages, IconManagerError>
    GetWindowImage(std::string_view path) const;

    std::expected<IconImage, IconManagerError> GetImage(std::string_view path, unsigned char& color);

    [[nodiscard]] std::expected<std::string, IconManagerError>
    GetSvg(std::string_view path) const;

private:
    std::filesystem::path icon_directory_;
};
