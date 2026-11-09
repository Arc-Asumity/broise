// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/render/Renderer.hpp
// Header file for render of UI.


#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>

#include <self/ui/window/Window.hpp>
#include <self/ui/Base.hpp>
#include <self/ui/Style.hpp>
#include <self/core/IconManager.hpp>
#include <self/core/WindowManager.hpp>


class Renderer
{
public:
    [[nodiscard]] static std::expected<Renderer, WindowError>
    Create(WindowManager& window_manager, Style style,
           std::filesystem::path icon_directory);

    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    void Init();
    void RenderPolygon(const Window& window, const PointSet& points,
                       const Color& color) const;
    void RenderIcon(const Window& window, std::string_view name, float x, float y,
                    float width, float height, const Color& color);
    [[nodiscard]] const Style& GetStyle() const noexcept;
    [[nodiscard]] IconManager& GetIconManager() noexcept;
    [[nodiscard]] const IconManager& GetIconManager() const noexcept;

private:

    Style style_;
    IconManager icon_manager_;

    unsigned int program_{};
    unsigned int vao_{};
    unsigned int vbo_{};
    std::unordered_map<std::string, unsigned int> icon_textures_;

    Renderer(Style style,
             std::filesystem::path icon_directory);
};
