// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Renderer.hpp
// Header file for render of UI.


#pragma once

#include <expected>

#include <self/ui/Window.hpp>
#include <self/ui/Base.hpp>
#include <self/ui/Style.hpp>


class Renderer
{
public:
    [[nodiscard]] static std::expected<Renderer, WindowError>
    Create(const char* title, Style style);

    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    void Init();
    void RenderPolygon(const PointSet& points, const Color& color) const;
    [[nodiscard]] bool ShouldClose() const;
    void PollEvents() const;
    void UpdateWindowInput();
    void SwapBuffers() const;
    [[nodiscard]] const Window& GetWindow() const noexcept;

private:

    Window window_;
    Style style_;

    unsigned int program_{};
    unsigned int vao_{};
    unsigned int vbo_{};

    Renderer(Window window, Style style);
};
