// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/window/WindowStatus.hpp
// State flags describing a window.

#pragma once

struct WindowStatus
{
    bool focused{};
    bool minimized{};
    bool maximized{};
    bool visible{};
    bool should_close{};
};
