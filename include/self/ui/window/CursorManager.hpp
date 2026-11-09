// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/ui/window/CursorManager.hpp
// Tracks and applies the cursor status for a window.

#pragma once

#include <GLFW/glfw3.h>

enum class CursorStatus
{
    Default,
    ResizeHorizontal,
    ResizeVertical,
    ResizeNorthWestSouthEast,
    ResizeNorthEastSouthWest,
};

class CursorManager
{
public:
    CursorManager() = default;
    ~CursorManager();
    CursorManager(const CursorManager&) = delete;
    CursorManager& operator=(const CursorManager&) = delete;
    CursorManager(CursorManager&& other) noexcept;
    CursorManager& operator=(CursorManager&& other) noexcept;

    [[nodiscard]] bool Initialize();
    void Shutdown() noexcept;
    void Apply(GLFWwindow* window, int hovered_edges);

    [[nodiscard]] CursorStatus GetStatus() const noexcept;

private:
    CursorStatus status_{CursorStatus::Default};
    GLFWcursor* horizontal_resize_cursor_{};
    GLFWcursor* vertical_resize_cursor_{};
    GLFWcursor* diagonal_resize_cursor_{};
    GLFWcursor* anti_diagonal_resize_cursor_{};
};
