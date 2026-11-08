// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/IconManager.cpp
// Loads and caches SVG source for Lucide icons.

#include <fstream>
#include <iterator>
#include <cstdlib>
#include <string>
#include <string_view>
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

std::expected<PointSet, std::string> IconManager::GetSegments(
    const std::string_view name)
{
    auto svg = GetSvg(name);
    if (!svg) {
        return std::unexpected(svg.error());
    }

    PointSet segments;
    std::size_t search_position = 0;
    while ((search_position = svg->find(" d=\"", search_position))
           != std::string::npos) {
        search_position += 4;
        const std::size_t data_end = svg->find('"', search_position);
        if (data_end == std::string::npos) {
            return std::unexpected("Malformed Lucide SVG path: " + std::string{name});
        }

        char* cursor = svg->data() + search_position;
        char* const end = svg->data() + data_end;
        char command = 0;
        float x = 0.0f;
        float y = 0.0f;
        while (cursor < end) {
            while (cursor < end && (*cursor == ' ' || *cursor == ',')) {
                ++cursor;
            }
            if (cursor == end) {
                break;
            }
            if ((*cursor >= 'A' && *cursor <= 'Z')
                || (*cursor >= 'a' && *cursor <= 'z')) {
                command = *cursor++;
                if (command == 'Z' || command == 'z') {
                    command = 0;
                    continue;
                }
            }
            if (command == 0) {
                return std::unexpected("Unsupported Lucide SVG path command: "
                                       + std::string{name});
            }

            char* number_end = cursor;
            const float first = std::strtof(cursor, &number_end);
            if (number_end == cursor || number_end > end) {
                return std::unexpected("Invalid Lucide SVG path data: "
                                       + std::string{name});
            }
            cursor = number_end;
            const bool relative = command >= 'a' && command <= 'z';
            const bool move_command = command == 'M' || command == 'm';
            char operation = command;
            if (operation == 'M') operation = 'L';
            if (operation == 'm') operation = 'l';
            float next_x = x;
            float next_y = y;
            if (operation == 'H' || operation == 'h') {
                next_x = relative ? x + first : first;
            } else if (operation == 'V' || operation == 'v') {
                next_y = relative ? y + first : first;
            } else if (operation == 'L' || operation == 'l') {
                while (cursor < end && (*cursor == ' ' || *cursor == ',')) ++cursor;
                number_end = cursor;
                const float second = std::strtof(cursor, &number_end);
                if (number_end == cursor || number_end > end) {
                    return std::unexpected("Invalid Lucide SVG path data: "
                                           + std::string{name});
                }
                cursor = number_end;
                next_x = relative ? x + first : first;
                next_y = relative ? y + second : second;
            } else {
                return std::unexpected("Unsupported Lucide SVG path command: "
                                       + std::string{name});
            }
            if (!move_command && (operation == 'L' || operation == 'l'
                || operation == 'H' || operation == 'h'
                || operation == 'V' || operation == 'v')) {
                segments.push_back({x / 24.0f, y / 24.0f});
                segments.push_back({next_x / 24.0f, next_y / 24.0f});
            }
            x = next_x;
            y = next_y;
            if (move_command) {
                command = relative ? 'l' : 'L';
            }
        }
        search_position = data_end + 1;
    }

    if (segments.empty()) {
        return std::unexpected("Lucide SVG contains no supported line segments: "
                               + std::string{name});
    }
    return segments;
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
