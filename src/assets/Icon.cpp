// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/assets/Icon.cpp
// Loads and caches SVG source for Lucide icons.


#include <fstream>
#include <lunasvg.h>

#include <self/assets/Icon.hpp>

IconManager::IconManager(const std::filesystem::path& icons_path)
{
    icon_directory_ = icons_path;
}

std::expected<std::vector<IconImage>, IconManagerError> IconManager::GetWindowImage(const std::string_view path) const
{
    std::string data;

    if (auto result = GetSvg(path))
        data = result.value();
    else
        return std::unexpected(result.error());

    const auto document = lunasvg::Document::loadFromData(data);
    if (document == nullptr)
        return std::unexpected(IconManagerError::LoadDocumentFailed);

    std::vector<IconImage> images;
    images.reserve(kWindowIconSize.size());

    for (const int& size : kWindowIconSize)
    {
        auto bitmap = document->renderToBitmap(size, size);
        if (bitmap.isNull())
            return std::unexpected(IconManagerError::LoadDocumentFailed);

        bitmap.convertToRGBA();

        const auto* pixels = bitmap.data();
        const auto pixels_size = bitmap.width() * bitmap.height() * 4;

        images.emplace_back(
            bitmap.width(), bitmap.height(),
            std::vector(pixels, pixels + pixels_size)
        );
    }

    return images;
}

std::expected<std::string, IconManagerError> IconManager::GetSvg(const std::string_view path) const
{
    std::ifstream file(icon_directory_ / path, std::ios::binary);
    if (!file)
        return std::unexpected(IconManagerError::NotFound);

    std::string svg{
        std::istreambuf_iterator{file},
        std::istreambuf_iterator<char>{}
    };
    if (file.bad())
        return std::unexpected(IconManagerError::ReadFailed);

    return svg;
}
