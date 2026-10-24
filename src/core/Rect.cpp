// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Rect.cpp
// Function for rect.


#include <numeric>

#include <self/core/Rect.hpp>

std::vector<Rect> Rect::SplitXs(const std::vector<float>& xs) const
{
    std::vector<Rect> result(xs.size());
    SplitXs(xs.data(), result.data(), xs.size());
    return result;
}

std::vector<Rect> Rect::SplitYs(const std::vector<float>& ys) const
{
    std::vector<Rect> result(ys.size());
    SplitYs(ys.data(), result.data(), ys.size());
    return result;
}

std::vector<Rect> Rect::SplitXs(const float* xs, const size_t size) const
{
    std::vector<Rect> result(size);
    SplitXs(xs, result.data(), size);
    return result;
}

std::vector<Rect> Rect::SplitYs(const float* ys, const size_t size) const
{
    std::vector<Rect> result(size);
    SplitYs(ys, result.data(), size);
    return result;
}

void Rect::SplitXs(const float* xs, Rect* result, const size_t size) const
{
    if (size == 0) return;
    const float sum = std::accumulate(xs, xs + size, 0.0f);
    if (sum <= 0.0f) return;
    float local_x = this->x;
    const float coefficient = this->width / sum;
    for (const float* i = xs; i < xs + size; i++)
    {
        const float current_width = *i * coefficient;
        *result = Rect{.x = local_x, .y = this->y, .width = current_width, .height = this->height};
        result++;
        local_x += current_width;
    }
}

void Rect::SplitYs(const float* ys, Rect* result, const size_t size) const
{
    if (size == 0) return;
    const float sum = std::accumulate(ys, ys + size, 0.0f);
    if (sum <= 0.0f) return;
    float local_y = this->y;
    const float coefficient = this->height / sum;
    for (const float* i = ys; i < ys + size; i++)
    {
        const float current_height = *i * coefficient;
        *result = Rect{.x = this->x, .y = local_y, .width = this->width, .height = current_height};
        result++;
        local_y += current_height;
    }
}

Point Rect::Translate(const float current_x, const float current_y) const
{
    Point result{};
    result.x = this->x + this->width * current_x;
    result.y = this->y + this->height * current_y;
    return result;
}

Point Rect::Translate(const Point point) const
{
    return Translate(point.x, point.y);
}

Rect Rect::operator*(const Rect& other) const
{
    return Rect{
        .x = x + other.x * width,
        .y = y + other.y * height,
        .width = other.width * width,
        .height = other.height * height
    };
}

std::ostream& operator<<(std::ostream& os, const Rect& rect)
{
    return os << "Rect{ x=" << rect.x
              << ", y=" << rect.y
              << ", width=" << rect.width
              << ", height=" << rect.height
              << " }";
}
