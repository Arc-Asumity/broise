// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/render/FontRenderer.hpp
// FreeType-backed font rendering.


#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <self/ui/Base.hpp>
#include <self/ui/Font.hpp>
#include <self/ui/Base.hpp>

class FontRenderer
{
public:
    FontRenderer();
    ~FontRenderer();

    FontRenderer(const FontRenderer&) = delete;
    FontRenderer& operator=(const FontRenderer&) = delete;

    void Render(std::string_view text, const Font& font,
                float x, float y, float width, float height,
                const Color& color);

private:
    struct FaceData
    {
        FT_Face face{};
        unsigned int pixel_size{};
    };

    struct Glyph
    {
        unsigned int texture{};
        int width{};
        int height{};
        int bearing_x{};
        int bearing_y{};
        unsigned int advance{};
    };

    FT_Library library_{};
    unsigned int program_{};
    unsigned int vao_{};
    unsigned int vbo_{};
    std::unordered_map<std::string, FaceData> faces_;
    std::unordered_map<std::string, Glyph> glyphs_;

    FaceData& GetFace(const Font& font);
    Glyph& GetGlyph(const std::string& key, FaceData& face, unsigned int codepoint);
};
