// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /include/self/ui/window/WindowContent.hpp
// Renders the visual content owned by a window.

#pragma once

#include <self/ui/window/CursorManager.hpp>

class Renderer;
class Window;
struct Style;

class WindowContent
{
public:
    void UpdateTitleBarInput(Window& window, CursorManager& cursor_manager);
    void Render(Window& window, Renderer& renderer, const Style& style) const;
};
