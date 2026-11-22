// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /include/self/ui/render/IconRenderer.hpp
// Draws rasterized icons and owns their GPU textures.

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <self/core/IconManager.hpp>
#include <self/ui/Base.hpp>
#include <self/ui/window/Window.hpp>

class IconRenderer
{
public:
    ~IconRenderer();
    IconRenderer() = default;
    IconRenderer(const IconRenderer&) = delete;
    IconRenderer& operator=(const IconRenderer&) = delete;
    IconRenderer(IconRenderer&& other) noexcept;
    IconRenderer& operator=(IconRenderer&& other) noexcept;

    void Render(const Window& window, IconManager& icon_manager,
                unsigned int program, unsigned int vao, unsigned int vbo,
                std::string_view name, float x, float y, float width,
                float height, const Color& color);

private:
    std::unordered_map<std::string, unsigned int> icon_textures_;
};
