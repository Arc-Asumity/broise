// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/ui/Style.cpp
// Style configuration parser.


#include <array>
#include <string>
#include <toml++/toml.hpp>

#include <self/ui/Style.hpp>

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

    std::expected<Font, std::string> readFont(
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

        return Font{.file = std::filesystem::path{*file}, .size = static_cast<float>(*size)};
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

    const auto* colors = broise->get_as<toml::table>("colors");
    const auto* fonts = broise->get_as<toml::table>("fonts");
    if (colors == nullptr || fonts == nullptr) {
        return std::unexpected(
            "Style configuration requires [broise.colors] "
            "and [broise.fonts] tables");
    }

    auto background = readColor(*colors, "background");
    auto bar = readColor(*colors, "bar");
    auto button = readColor(*colors, "button");
    auto hover = readColor(*colors, "hover");
    auto text = readColor(*colors, "text");
    if (!background) return std::unexpected(background.error());
    if (!bar) return std::unexpected(bar.error());
    if (!button) return std::unexpected(button.error());
    if (!hover) return std::unexpected(hover.error());
    if (!text) return std::unexpected(text.error());
    Style style;
    style.colors = {
        .background = *background,
        .bar = *bar,
        .button = *button,
        .hover = *hover,
        .text = *text,
    };
    const std::filesystem::path install_directory =
        path.parent_path().parent_path();
    const auto resolve_font_path = [&install_directory](Font& font) {
        if (font.file.is_relative()) {
            font.file = install_directory / font.file;
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

    for (const std::string role : {"content", "title", "maths"}) {
        if (!style.fonts.contains(role)) {
            return std::unexpected("Missing required font role '" + role + "'");
        }
    }
    return style;
}
