// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/Base.hpp
// Basic definition for Broise.

#pragma once

#include <vector>
#include <ostream>

struct Point
{
    float x;
    float y;
};

using PointSet = std::vector<Point>;

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

inline std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << "Point{ x=" << point.x
              << ", y=" << point.y
              << " }";
}

inline std::ostream& operator<<(std::ostream& os, const std::vector<Point>& points)
{
    return os << points;
}
