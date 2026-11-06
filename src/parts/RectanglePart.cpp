// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/parts/RectanglePart.cpp
// Rectangle Part implementation.

#include <array>
#include <cmath>

#include <self/core/Renderer.hpp>
#include <self/parts/RectanglePart.hpp>

void RectanglePart::SetBounds(const Rect bounds)
{
    bounds_ = bounds;
}

void RectanglePart::SetColor(const Color color)
{
    color_ = color;
}

void RectanglePart::SetRotation(const float radians)
{
    rotation_ = radians;
}

void RectanglePart::Render(const PartRenderContext& context)
{
    if (context.logical_width <= 0.0f || context.logical_height <= 0.0f) {
        return;
    }

    const float center_x =
        (bounds_.x + bounds_.width / 2.0f) * context.logical_width;
    const float center_y =
        (bounds_.y + bounds_.height / 2.0f) * context.logical_height;
    const float half_width = bounds_.width * context.logical_width / 2.0f;
    const float half_height = bounds_.height * context.logical_height / 2.0f;
    const float cosine = std::cos(rotation_);
    const float sine = std::sin(rotation_);
    const std::array<Point, 4> corners = {{
        {-half_width, -half_height},
        {half_width, -half_height},
        {half_width, half_height},
        {-half_width, half_height},
    }};
    PointSet points;
    points.reserve(corners.size());

    for (const Point& corner : corners) {
        const float x = corner.x * cosine - corner.y * sine;
        const float y = corner.x * sine + corner.y * cosine;
        points.push_back({
            (center_x + x) / context.logical_width,
            (center_y + y) / context.logical_height,
        });
    }

    context.renderer.RenderPolygon(points, color_);
}

void RectanglePart::Update() {}
