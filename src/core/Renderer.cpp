// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Renderer.cpp
// Renderer the ui.


#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <glad/gl.h>

#include <self/core/Renderer.hpp>

Renderer::Renderer(Renderer&& other) noexcept
    : program_(std::exchange(other.program_, 0)),
      vao_(std::exchange(other.vao_, 0)),
      vbo_(std::exchange(other.vbo_, 0))
{}

Renderer& Renderer::operator=(Renderer&& other) noexcept
{
    if (this == &other) {
        return *this;
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

    program_ = std::exchange(other.program_, 0);
    vao_ = std::exchange(other.vao_, 0);
    vbo_ = std::exchange(other.vbo_, 0);

    return *this;
}

namespace
{
    constexpr char vertex_shader_source[] = R"(
        #version 460 core

        layout (location = 0) in vec2 a_position;

        void main()
        {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";

    constexpr char fragment_shader_source[] = R"(
        #version 460 core

        uniform vec4 u_color;

        out vec4 fragment_color;

        void main()
        {
            fragment_color = u_color;
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
        2 * sizeof(float),
        nullptr
    );

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
    }

    glUseProgram(program_);

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
