// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/Rect.hpp
// Header for rect.


#pragma once

#include <array>
#include <ostream>
#include <utility>
#include <vector>

#include <self/ui/Base.hpp>

struct Rect
{
    float x;
    float y;
    float width;
    float height;

    [[nodiscard]] constexpr std::pair<Rect, Rect>
    SplitX(const float left, const float right) const
    {
        Rect left_rect = *this;
        left_rect.width = left * this->width / (left + right);
        Rect right_rect = *this;
        right_rect.width = this->width - left_rect.width;
        right_rect.x = left_rect.x + left_rect.width;
        return {left_rect, right_rect};
    }
    [[nodiscard]] constexpr  std::pair<Rect, Rect>
    SplitY(const float top, const float bottom) const
    {
        Rect top_rect = *this;
        top_rect.height = top * this->height / (top + bottom);
        Rect bottom_rect = *this;
        bottom_rect.height = this->height - top_rect.height;
        bottom_rect.y = top_rect.y + top_rect.height;
        return {top_rect, bottom_rect};
    }

    [[nodiscard]] std::vector<Rect>
    SplitXs(const std::vector<float>& xs) const;
    [[nodiscard]] std::vector<Rect>
    SplitYs(const std::vector<float>& ys) const;


    template <const size_t N>
    std::array<Rect, N> SplitXs(const std::array<float, N>& xs) const
    {
        std::array<Rect, N> result;
        SplitXs(xs.data(), result.data(), N);
        return result;
    }
    template <const size_t N>
    std::array<Rect, N> SplitYs(const std::array<float, N>& ys) const
    {
        std::array<Rect, N> result;
        SplitYs(ys.data(), result.data(), N);
        return result;
    }

    [[nodiscard]] std::vector<Rect>
    SplitXs(const float* xs, size_t size) const;
    [[nodiscard]] std::vector<Rect>
    SplitYs(const float* ys, size_t size) const;

    [[nodiscard]] Point
    Translate(float current_x, float current_y) const;
    [[nodiscard]] Point
    Translate(Point point) const;

    Rect operator*(const Rect& other) const;

private:
    void SplitXs(const float* xs, Rect* result, size_t size) const;
    void SplitYs(const float* ys, Rect* result, size_t size) const;
};

std::ostream& operator<<(std::ostream& os, const Rect& rect);

constexpr auto kFullRect = Rect{
    .x = 0.0f,
    .y = 0.0f,
    .width = 1.0f,
    .height = 1.0f
};
