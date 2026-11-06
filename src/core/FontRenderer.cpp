#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include <glad/gl.h>

#include "FontRenderer.hpp"

namespace
{
    constexpr char kVertexShader[] = R"(
        #version 460 core
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_uv;
        out vec2 v_uv;
        void main() {
            gl_Position = vec4(a_position.x * 2.0 - 1.0,
                               1.0 - a_position.y * 2.0, 0.0, 1.0);
            v_uv = a_uv;
        }
    )";

    constexpr char kFragmentShader[] = R"(
        #version 460 core
        in vec2 v_uv;
        uniform sampler2D u_glyph;
        uniform vec4 u_color;
        out vec4 fragment_color;
        void main() {
            fragment_color = vec4(u_color.rgb, u_color.a * texture(u_glyph, v_uv).r);
        }
    )";

    GLuint CompileShader(GLenum type, const char* source)
    {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glDeleteShader(shader);
            throw std::runtime_error("Failed to compile text rendering shader");
        }
        return shader;
    }
}

FontRenderer::FontRenderer()
{
    if (FT_Init_FreeType(&library_) != 0) {
        throw std::runtime_error("Failed to initialize FreeType");
    }

    const GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, kVertexShader);
    const GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint success = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        throw std::runtime_error("Failed to link text rendering shader");
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

FontRenderer::~FontRenderer()
{
    for (const auto& [key, glyph] : glyphs_) {
        glDeleteTextures(1, &glyph.texture);
    }
    for (auto& [path, data] : faces_) {
        FT_Done_Face(data.face);
    }
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (program_ != 0) glDeleteProgram(program_);
    if (library_ != nullptr) FT_Done_FreeType(library_);
}

FontRenderer::FaceData& FontRenderer::GetFace(const FontStyle& font)
{
    const std::string path = font.file.string();
    auto [iterator, inserted] = faces_.try_emplace(path);
    if (inserted && FT_New_Face(library_, path.c_str(), 0, &iterator->second.face) != 0) {
        faces_.erase(iterator);
        throw std::runtime_error("Failed to load font file: " + path);
    }

    const auto pixel_size = static_cast<unsigned int>(std::lround(font.size));
    if (iterator->second.pixel_size != pixel_size) {
        if (FT_Set_Pixel_Sizes(iterator->second.face, 0, pixel_size) != 0) {
            throw std::runtime_error("Failed to set font size for: " + path);
        }
        iterator->second.pixel_size = pixel_size;
    }
    return iterator->second;
}

FontRenderer::Glyph& FontRenderer::GetGlyph(
    const std::string& key, FaceData& face, const unsigned int codepoint)
{
    auto [iterator, inserted] = glyphs_.try_emplace(key);
    if (!inserted) return iterator->second;

    if (FT_Load_Char(face.face, codepoint, FT_LOAD_RENDER) != 0) {
        glyphs_.erase(iterator);
        throw std::runtime_error("Failed to rasterize font glyph");
    }

    const FT_Bitmap& bitmap = face.face->glyph->bitmap;
    Glyph& glyph = iterator->second;
    glyph.width = static_cast<int>(bitmap.width);
    glyph.height = static_cast<int>(bitmap.rows);
    glyph.bearing_x = face.face->glyph->bitmap_left;
    glyph.bearing_y = face.face->glyph->bitmap_top;
    glyph.advance = static_cast<unsigned int>(face.face->glyph->advance.x >> 6);

    if (glyph.width == 0 || glyph.height == 0) return glyph;

    std::vector<unsigned char> pixels(static_cast<std::size_t>(glyph.width * glyph.height));
    for (int row = 0; row < glyph.height; ++row) {
        const int source_row = bitmap.pitch >= 0 ? row : glyph.height - row - 1;
        const auto* source = bitmap.buffer + source_row * std::abs(bitmap.pitch);
        std::copy_n(source, glyph.width, pixels.data() + row * glyph.width);
    }

    glGenTextures(1, &glyph.texture);
    glBindTexture(GL_TEXTURE_2D, glyph.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, glyph.width, glyph.height,
                 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    return glyph;
}

void FontRenderer::Render(
    const std::string_view text, const FontStyle& font,
    const float x, const float y, const float width, const float height,
    const Color& color)
{
    if (width <= 0.0f || height <= 0.0f) return;

    FaceData& face = GetFace(font);
    const float scale_x = 1.0f / width;
    const float scale_y = 1.0f / height;
    const float ascender = static_cast<float>(face.face->size->metrics.ascender >> 6);
    const float baseline = (y + ascender) * scale_y;
    float pen_x = x * scale_x;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program_);
    glUniform4f(glGetUniformLocation(program_, "u_color"),
                color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(program_, "u_glyph"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_);

    for (const unsigned char codepoint : text) {
        const std::string key = font.file.string() + ":" + std::to_string(font.size)
                              + ":" + std::to_string(codepoint);
        Glyph& glyph = GetGlyph(key, face, codepoint);
        const float left = pen_x + static_cast<float>(glyph.bearing_x) * scale_x;
        const float top = baseline - static_cast<float>(glyph.bearing_y) * scale_y;
        const float right = left + static_cast<float>(glyph.width) * scale_x;
        const float bottom = top + static_cast<float>(glyph.height) * scale_y;
        const std::array<float, 24> vertices = {
            left, top, 0.0f, 0.0f, left, bottom, 0.0f, 1.0f,
            right, bottom, 1.0f, 1.0f, left, top, 0.0f, 0.0f,
            right, bottom, 1.0f, 1.0f, right, top, 1.0f, 0.0f,
        };

        if (glyph.texture != 0) {
            glBindTexture(GL_TEXTURE_2D, glyph.texture);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STREAM_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        pen_x += static_cast<float>(glyph.advance) * scale_x;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
