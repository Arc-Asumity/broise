// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Style.cpp
// Style configuration parser.

#include <array>
#include <string>
#include <toml++/toml.hpp>

#include <self/core/Style.hpp>

namespace
{
    std::expected<Color, std::string> readColor(
        const toml::table& colors, const std::string& name)
    {
        const auto* values = colors.get_as<toml::array>(name);
        if (values == nullptr || values->size() != 4) {
            return std::unexpected("Color '" + name + "' must be an RGBA array");
        }

        std::array<float, 4> channels{};
        for (std::size_t index = 0; index < channels.size(); ++index) {
            const auto value = (*values)[index].value<double>();
            if (!value || *value < 0.0 || *value > 1.0) {
                return std::unexpected("Color '" + name + "' channels must be between 0 and 1");
            }
            channels[index] = static_cast<float>(*value);
        }

        return Color{channels[0], channels[1], channels[2], channels[3]};
    }

    std::expected<FontStyle, std::string> readFont(
        const toml::table& fonts, const std::string& name)
    {
        const auto* font = fonts.get_as<toml::table>(name);
        if (font == nullptr) {
            return std::unexpected("Missing font style '" + name + "'");
        }

        const auto file = (*font)["file"].value<std::string>();
        const auto size = (*font)["size"].value<double>();
        if (!file || file->empty() || !size || *size <= 0.0) {
            return std::unexpected("Font style '" + name + "' needs a file and positive size");
        }

        return FontStyle{.file = std::filesystem::path{*file}, .size = static_cast<float>(*size)};
    }
}

std::expected<Style, std::string>
Style::Load(const std::filesystem::path& path)
{
    toml::table config;
    try {
        config = toml::parse_file(path.string());
    } catch (const toml::parse_error& error) {
        return std::unexpected(std::string{error.description()});
    }

    const auto* broise = config.get_as<toml::table>("broise");
    if (broise == nullptr) {
        return std::unexpected("Style configuration requires a [broise] table");
    }

    const auto* colors = broise->get_as<toml::table>("style");
    const auto* bar = colors == nullptr ? nullptr : colors->get_as<toml::table>("bar");
    const auto* fonts = broise->get_as<toml::table>("fonts");
    if (colors == nullptr || bar == nullptr || fonts == nullptr) {
        return std::unexpected(
            "Style configuration requires [broise.style], [broise.style.bar], "
            "and [broise.fonts] tables");
    }

    auto background = readColor(*colors, "background");
    auto title_bar = readColor(*bar, "background");
    auto title_button = readColor(*bar, "button");
    auto close_button = readColor(*bar, "close_button");
    auto title_icon = readColor(*bar, "icon");
    if (!background) return std::unexpected(background.error());
    if (!title_bar) return std::unexpected(title_bar.error());
    if (!title_button) return std::unexpected(title_button.error());
    if (!close_button) return std::unexpected(close_button.error());
    if (!title_icon) return std::unexpected(title_icon.error());
    Style style;
    style.background_color = *background;
    style.title_bar_color = *title_bar;
    style.title_bar_button_color = *title_button;
    style.close_button_color = *close_button;
    style.title_bar_icon_color = *title_icon;
    const auto resolve_font_path = [&path](FontStyle& font) {
        if (font.file.is_relative()) {
            font.file = path.parent_path() / font.file;
        }
    };
    for (const auto& [name, value] : *fonts) {
        const std::string role{name.str()};
        if (!value.is_table()) {
            return std::unexpected("Font role '" + role + "' must be a table");
        }
        auto font = readFont(*fonts, role);
        if (!font) return std::unexpected(font.error());
        resolve_font_path(*font);
        style.fonts.emplace(role, *font);
    }

    for (const std::string role : {"common", "title", "latex"}) {
        if (!style.fonts.contains(role)) {
            return std::unexpected("Missing required font role '" + role + "'");
        }
    }
    return style;
}
