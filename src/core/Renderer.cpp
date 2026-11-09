// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/ui/Renderer.cpp
// Renderer the ui.


#include <stdexcept>
#include <utility>
#include <vector>
#include <string>
#include <glad/gl.h>
#include <lunasvg.h>

#include <self/core/Renderer.hpp>
#include <self/ui/Font.hpp>

Renderer::Renderer(Window& window, Style style,
                   std::filesystem::path icon_directory)
    : window_(&window)
    , style_(std::move(style))
    , icon_manager_(std::move(icon_directory))
{}

std::expected<Renderer, WindowError> Renderer::Create(
    Window& window, Style style, std::filesystem::path icon_directory)
{
    Renderer renderer(window, std::move(style),
                      std::move(icon_directory));
    if (auto icon = renderer.icon_manager_.GetIcon("broise")) {
        auto document = lunasvg::Document::loadFromData(icon->svg);
        if (document) {
            auto bitmap = document->renderToBitmap(64, 64);
            if (!bitmap.isNull()) {
                bitmap.convertToRGBA();
                const GLFWimage window_icon{
                    .width = bitmap.width(),
                    .height = bitmap.height(),
                    .pixels = bitmap.data(),
                };
                renderer.window_->SetIcon(window_icon);
            }
        }
    }
    renderer.Init();
    return renderer;
}

Renderer::Renderer(Renderer&& other) noexcept
    : window_(std::exchange(other.window_, nullptr)),
      style_(std::move(other.style_)),
      icon_manager_(std::move(other.icon_manager_)),
      program_(std::exchange(other.program_, 0)),
      vao_(std::exchange(other.vao_, 0)),
      vbo_(std::exchange(other.vbo_, 0)),
      icon_textures_(std::move(other.icon_textures_))
{}

Renderer& Renderer::operator=(Renderer&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    for (const auto& [name, texture] : icon_textures_) {
        glDeleteTextures(1, &texture);
    }

    window_ = std::exchange(other.window_, nullptr);
    style_ = std::move(other.style_);
    icon_manager_ = std::move(other.icon_manager_);

    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }

    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }

    if (program_ != 0) {
        glDeleteProgram(program_);
    }

    program_ = std::exchange(other.program_, 0);
    vao_ = std::exchange(other.vao_, 0);
    vbo_ = std::exchange(other.vbo_, 0);
    icon_textures_ = std::move(other.icon_textures_);

    return *this;
}

namespace
{
    constexpr char vertex_shader_source[] = R"(
        #version 460 core

        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_uv;
        out vec2 v_uv;

        void main()
        {
            v_uv = a_uv;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";

    constexpr char fragment_shader_source[] = R"(
        #version 460 core

        uniform vec4 u_color;
        uniform sampler2D u_texture;
        uniform int u_use_texture;
        in vec2 v_uv;

        out vec4 fragment_color;

        void main()
        {
            fragment_color = u_use_texture != 0
                ? texture(u_texture, v_uv) * u_color : u_color;
        }
    )";

    GLuint compileShader(
        const GLenum type,
        const char* source
    )
    {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success{};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glDeleteShader(shader);
            throw std::runtime_error("Failed to compile OpenGL shader");
        }

        return shader;
    }
}

Renderer::~Renderer()
{
    for (const auto& [name, texture] : icon_textures_) {
        glDeleteTextures(1, &texture);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }

    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }

    if (program_ != 0) {
        glDeleteProgram(program_);
    }
}

void Renderer::Init()
{
    const GLuint vertex_shader =
        compileShader(GL_VERTEX_SHADER, vertex_shader_source);

    const GLuint fragment_shader =
        compileShader(GL_FRAGMENT_SHADER, fragment_shader_source);

    program_ = glCreateProgram();

    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint success{};
    glGetProgramiv(program_, GL_LINK_STATUS, &success);

    if (!success) {
        glDeleteProgram(program_);
        program_ = 0;
        throw std::runtime_error("Failed to link OpenGL shader program");
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        nullptr
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

#define GL_CHECK() \
    do { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            std::cerr << "OpenGL error " << error \
                      << " at line " << __LINE__ << '\n'; \
        } \
    } while (false)

void Renderer::RenderPolygon(const PointSet& points, const Color& color) const
{
    if (points.size() < 3) {
        return;
    }

    std::vector<float> vertices;
    vertices.reserve(points.size() * 2);

    for (const auto& [x, y] : points) {
        vertices.push_back(x * 2.0f - 1.0f);
        vertices.push_back(1.0f - y * 2.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
    }

    glUseProgram(program_);
    glUniform1i(glGetUniformLocation(program_, "u_use_texture"), 0);

    const GLint colorLocation =
    glGetUniformLocation(program_, "u_color");

    glUniform4f(
        colorLocation,
        color.r,
        color.g,
        color.b,
        color.a
    );

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
        vertices.data(),
        GL_STREAM_DRAW
    );

    glDrawArrays(
        GL_TRIANGLE_FAN,
        0,
        static_cast<GLsizei>(points.size())
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::RenderIcon(const std::string_view name, const float x,
                          const float y, const float width, const float height,
                          const Color& color)
{
    const std::string key{name};
    auto cached_texture = icon_textures_.find(key);
    if (cached_texture == icon_textures_.end()) {
        auto icon = icon_manager_.GetIcon(name);
        if (!icon) return;

        std::string svg = icon->svg;
        std::string::size_type position = 0;
        while ((position = svg.find("currentColor", position))
               != std::string::npos) {
            svg.replace(position, 12, "#ffffff");
            position += 7;
        }
        auto document = lunasvg::Document::loadFromData(svg);
        if (!document) return;
        auto bitmap = document->renderToBitmap(64, 64);
        if (bitmap.isNull()) return;
        bitmap.convertToRGBA();

        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap.width(), bitmap.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.data());
        glBindTexture(GL_TEXTURE_2D, 0);
        cached_texture = icon_textures_.emplace(key, texture).first;
    }

    const float logical_width = static_cast<float>(window_->GetLogicalWidth());
    const float logical_height = static_cast<float>(window_->GetLogicalHeight());
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
    glUseProgram(program_);
    glUniform4f(glGetUniformLocation(program_, "u_color"),
                color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(program_, "u_use_texture"), 1);
    glUniform1i(glGetUniformLocation(program_, "u_texture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cached_texture->second);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
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

bool Renderer::ShouldClose() const
{
    return window_->ShouldClose();
}

void Renderer::SwapBuffers() const
{
    window_->SwapBuffers();
}

const Window& Renderer::GetWindow() const noexcept
{
    return *window_;
}

const Style& Renderer::GetStyle() const noexcept
{
    return style_;
}

IconManager& Renderer::GetIconManager() noexcept
{
    return icon_manager_;
}

const IconManager& Renderer::GetIconManager() const noexcept
{
    return icon_manager_;
}
