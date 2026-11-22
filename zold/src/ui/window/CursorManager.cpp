// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/ui/window/CursorManager.cpp
// Tracks and applies the cursor status for a window.

#include <self/ui/window/CursorManager.hpp>
#include <utility>

CursorManager::~CursorManager()
{
    Shutdown();
}

CursorManager::CursorManager(CursorManager&& other) noexcept
    : status_(other.status_)
    , horizontal_resize_cursor_(
          std::exchange(other.horizontal_resize_cursor_, nullptr))
    , vertical_resize_cursor_(
          std::exchange(other.vertical_resize_cursor_, nullptr))
    , diagonal_resize_cursor_(
          std::exchange(other.diagonal_resize_cursor_, nullptr))
    , anti_diagonal_resize_cursor_(
          std::exchange(other.anti_diagonal_resize_cursor_, nullptr))
{}

CursorManager& CursorManager::operator=(CursorManager&& other) noexcept
{
    if (this == &other) {
        return *this;
    }
    Shutdown();
    status_ = other.status_;
    horizontal_resize_cursor_ =
        std::exchange(other.horizontal_resize_cursor_, nullptr);
    vertical_resize_cursor_ =
        std::exchange(other.vertical_resize_cursor_, nullptr);
    diagonal_resize_cursor_ =
        std::exchange(other.diagonal_resize_cursor_, nullptr);
    anti_diagonal_resize_cursor_ =
        std::exchange(other.anti_diagonal_resize_cursor_, nullptr);
    return *this;
}

bool CursorManager::Initialize()
{
    horizontal_resize_cursor_ = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    vertical_resize_cursor_ = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    diagonal_resize_cursor_ = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    anti_diagonal_resize_cursor_ = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    if (horizontal_resize_cursor_ == nullptr || vertical_resize_cursor_ == nullptr
        || diagonal_resize_cursor_ == nullptr
        || anti_diagonal_resize_cursor_ == nullptr) {
        Shutdown();
        return false;
    }
    return true;
}

void CursorManager::Shutdown() noexcept
{
    if (horizontal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(horizontal_resize_cursor_);
        horizontal_resize_cursor_ = nullptr;
    }
    if (vertical_resize_cursor_ != nullptr) {
        glfwDestroyCursor(vertical_resize_cursor_);
        vertical_resize_cursor_ = nullptr;
    }
    if (diagonal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(diagonal_resize_cursor_);
        diagonal_resize_cursor_ = nullptr;
    }
    if (anti_diagonal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(anti_diagonal_resize_cursor_);
        anti_diagonal_resize_cursor_ = nullptr;
    }
}

void CursorManager::Apply(GLFWwindow* window, const int hovered_edges)
{
    constexpr int kResizeLeft = 1;
    constexpr int kResizeRight = 2;
    constexpr int kResizeTop = 4;
    constexpr int kResizeBottom = 8;
    const bool horizontal = (hovered_edges & (kResizeLeft | kResizeRight)) != 0;
    const bool vertical = (hovered_edges & (kResizeTop | kResizeBottom)) != 0;

    GLFWcursor* cursor = nullptr;
    if (horizontal && vertical) {
        const bool same_diagonal =
            ((hovered_edges & kResizeLeft) != 0)
            == ((hovered_edges & kResizeTop) != 0);
        status_ = same_diagonal
            ? CursorStatus::ResizeNorthWestSouthEast
            : CursorStatus::ResizeNorthEastSouthWest;
        cursor = same_diagonal ? diagonal_resize_cursor_
                               : anti_diagonal_resize_cursor_;
    } else if (horizontal) {
        status_ = CursorStatus::ResizeHorizontal;
        cursor = horizontal_resize_cursor_;
    } else if (vertical) {
        status_ = CursorStatus::ResizeVertical;
        cursor = vertical_resize_cursor_;
    } else {
        status_ = CursorStatus::Default;
    }

    glfwSetCursor(window, cursor);
}

CursorStatus CursorManager::GetStatus() const noexcept
{
    return status_;
}
