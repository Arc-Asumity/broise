// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/ui/render/IconRenderer.cpp
// Draws rasterized icons and owns their GPU textures.

#include <utility>
#include <vector>

#include <glad/gl.h>

#include <self/ui/render/IconRenderer.hpp>

IconRenderer::~IconRenderer()
{
    for (const auto& [name, texture] : icon_textures_) {
        glDeleteTextures(1, &texture);
    }
}

IconRenderer::IconRenderer(IconRenderer&& other) noexcept
    : icon_textures_(std::move(other.icon_textures_))
{
    other.icon_textures_.clear();
}

IconRenderer& IconRenderer::operator=(IconRenderer&& other) noexcept
{
    if (this == &other) {
        return *this;
    }
    for (const auto& [name, texture] : icon_textures_) {
        glDeleteTextures(1, &texture);
    }
    icon_textures_ = std::move(other.icon_textures_);
    other.icon_textures_.clear();
    return *this;
}

void IconRenderer::Render(
    const Window& window, IconManager& icon_manager,
    const unsigned int program, const unsigned int vao, const unsigned int vbo,
    const std::string_view name, const float x, const float y,
    const float width, const float height, const Color& color)
{
    const std::string key{name};
    auto cached_texture = icon_textures_.find(key);
    if (cached_texture == icon_textures_.end()) {
        auto icon = icon_manager.GetIcon(name);
        if (!icon) return;

        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, icon->width, icon->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, icon->rgba.data());
        glBindTexture(GL_TEXTURE_2D, 0);
        cached_texture = icon_textures_.emplace(key, texture).first;
    }

    const float logical_width = static_cast<float>(window.GetLogicalWidth());
    const float logical_height = static_cast<float>(window.GetLogicalHeight());
    if (logical_width <= 0.0f || logical_height <= 0.0f) return;
    const float left = x / logical_width * 2.0f - 1.0f;
    const float right = (x + width) / logical_width * 2.0f - 1.0f;
    const float top = 1.0f - y / logical_height * 2.0f;
    const float bottom = 1.0f - (y + height) / logical_height * 2.0f;
    const std::vector<float> vertices{
        left, top, 0.0f, 1.0f, right, top, 1.0f, 1.0f,
        right, bottom, 1.0f, 0.0f, left, top, 0.0f, 1.0f,
        right, bottom, 1.0f, 0.0f, left, bottom, 0.0f, 0.0f,
    };
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program);
    glUniform4f(glGetUniformLocation(program, "u_color"),
                color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(program, "u_use_texture"), 1);
    glUniform1i(glGetUniformLocation(program, "u_texture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cached_texture->second);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glDisable(GL_BLEND);
}
