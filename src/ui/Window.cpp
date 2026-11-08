// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Window.cpp
// Window for Broise.


#include <glad/gl.h>
#include <string_view>
#include <utility>

#include <self/ui/Window.hpp>
#include <self/core/Renderer.hpp>
#include <self/ui/Style.hpp>

std::expected<Window, WindowError>
Window::Create(const char* title)
{
    Window self;

    if (glfwInit() == GLFW_FALSE) {
        return std::unexpected(WindowError::GlfwInitializationFailed);
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode =
        glfwGetVideoMode(monitor);

    if (monitor == nullptr || mode == nullptr) {
        glfwTerminate();
        return std::unexpected(
            WindowError::WindowCreationFailed
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    auto* window = glfwCreateWindow(
        static_cast<int>(static_cast<float>(mode->width) * kScale),
        static_cast<int>(static_cast<float>(mode->height) * kScale),
        title,
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        glfwTerminate();
        return std::unexpected(WindowError::WindowCreationFailed);
    }

    self.horizontal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    self.vertical_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    self.diagonal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    self.anti_diagonal_resize_cursor_ =
        glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

    int monitorX = 0;
    int monitorY = 0;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    glfwSetWindowPos(
        window,
        monitorX + static_cast<int>(static_cast<float>(mode->width) * (1 - kScale) / 2),
        monitorY + static_cast<int>(static_cast<float>(mode->height) * (1 - kScale) / 2)
    );

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return std::unexpected(
            WindowError::OpenGLLoaderInitializationFailed
        );
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(
        window,
        &framebufferWidth,
        &framebufferHeight
    );

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    self.width_ = framebufferWidth;
    self.height_ = framebufferHeight;

    glfwSetWindowUserPointer(window, &self);
    glfwSetFramebufferSizeCallback(
        window,
        FramebufferSizeCallback
    );

    self.window_ = window;

    return self;
}

Window::~Window()
{
    if (horizontal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(horizontal_resize_cursor_);
    }
    if (vertical_resize_cursor_ != nullptr) {
        glfwDestroyCursor(vertical_resize_cursor_);
    }
    if (diagonal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(diagonal_resize_cursor_);
    }
    if (anti_diagonal_resize_cursor_ != nullptr) {
        glfwDestroyCursor(anti_diagonal_resize_cursor_);
    }
    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(window_);
}

void Window::UpdateTitleBarInput()
{
    const bool left_mouse_is_down =
        glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window_, &cursor_x, &cursor_y);

    int window_width = 0;
    int window_height = 0;
    glfwGetWindowSize(window_, &window_width, &window_height);

    int window_x = 0;
    int window_y = 0;
    glfwGetWindowPos(window_, &window_x, &window_y);

    const auto title_bar_height = static_cast<double>(GetTitleBarHeight());
    const auto control_width = static_cast<double>(GetTitleBarControlWidth());
    constexpr double kResizeBorder = 7.0;
    constexpr int kResizeLeft = 1;
    constexpr int kResizeRight = 2;
    constexpr int kResizeTop = 4;
    constexpr int kResizeBottom = 8;

    int hovered_edges = resizing_ ? resize_edges_ : 0;
    if (!resizing_) {
        if (cursor_x < kResizeBorder) hovered_edges |= kResizeLeft;
        if (cursor_x >= window_width - kResizeBorder) hovered_edges |= kResizeRight;
        if (cursor_y < kResizeBorder) hovered_edges |= kResizeTop;
        if (cursor_y >= window_height - kResizeBorder) hovered_edges |= kResizeBottom;
    }
    GLFWcursor* cursor = nullptr;
    if ((hovered_edges & (kResizeLeft | kResizeRight)) != 0
        && (hovered_edges & (kResizeTop | kResizeBottom)) != 0) {
        const bool same_diagonal =
            ((hovered_edges & kResizeLeft) != 0)
                == ((hovered_edges & kResizeTop) != 0);
        cursor = same_diagonal ? diagonal_resize_cursor_
                               : anti_diagonal_resize_cursor_;
    } else if ((hovered_edges & (kResizeLeft | kResizeRight)) != 0) {
        cursor = horizontal_resize_cursor_;
    } else if ((hovered_edges & (kResizeTop | kResizeBottom)) != 0) {
        cursor = vertical_resize_cursor_;
    }
    glfwSetCursor(window_, cursor);

    if (left_mouse_is_down && !left_mouse_was_down_) {
        resize_edges_ = hovered_edges;

        if (resize_edges_ != 0) {
            resizing_ = true;
            dragging_title_bar_ = false;
            drag_start_window_x_ = window_x;
            drag_start_window_y_ = window_y;
            resize_start_width_ = window_width;
            resize_start_height_ = window_height;
            drag_start_cursor_x_ = static_cast<double>(window_x) + cursor_x;
            drag_start_cursor_y_ = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (left_mouse_is_down && !left_mouse_was_down_ && !resizing_
        && cursor_y < title_bar_height) {
        if (cursor_x >= window_width - control_width) {
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        } else if (cursor_x >= window_width - 2.0 * control_width) {
            if (glfwGetWindowAttrib(window_, GLFW_MAXIMIZED) == GLFW_TRUE) {
                glfwRestoreWindow(window_);
            } else {
                glfwMaximizeWindow(window_);
            }
        } else if (cursor_x >= window_width - 3.0 * control_width) {
            glfwIconifyWindow(window_);
        } else {
            dragging_title_bar_ = true;
            drag_start_window_x_ = window_x;
            drag_start_window_y_ = window_y;
            drag_start_cursor_x_ = static_cast<double>(window_x) + cursor_x;
            drag_start_cursor_y_ = static_cast<double>(window_y) + cursor_y;
        }
    }

    if (!left_mouse_is_down) {
        dragging_title_bar_ = false;
        resizing_ = false;
        resize_edges_ = 0;
    } else if (resizing_) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        const int delta_x = static_cast<int>(cursor_screen_x - drag_start_cursor_x_);
        const int delta_y = static_cast<int>(cursor_screen_y - drag_start_cursor_y_);

        int width = resize_start_width_;
        int height = resize_start_height_;
        int x = drag_start_window_x_;
        int y = drag_start_window_y_;
        if ((resize_edges_ & kResizeRight) != 0) width += delta_x;
        if ((resize_edges_ & kResizeLeft) != 0) {
            width -= delta_x;
            x += delta_x;
        }
        if ((resize_edges_ & kResizeBottom) != 0) height += delta_y;
        if ((resize_edges_ & kResizeTop) != 0) {
            height -= delta_y;
            y += delta_y;
        }

        if (width < 320) {
            if ((resize_edges_ & kResizeLeft) != 0) x -= 320 - width;
            width = 320;
        }
        if (height < 200) {
            if ((resize_edges_ & kResizeTop) != 0) y -= 200 - height;
            height = 200;
        }
        glfwSetWindowPos(window_, x, y);
        glfwSetWindowSize(window_, width, height);
    } else if (dragging_title_bar_) {
        const double cursor_screen_x = static_cast<double>(window_x) + cursor_x;
        const double cursor_screen_y = static_cast<double>(window_y) + cursor_y;
        glfwSetWindowPos(
            window_,
            drag_start_window_x_ + static_cast<int>(cursor_screen_x - drag_start_cursor_x_),
            drag_start_window_y_ + static_cast<int>(cursor_screen_y - drag_start_cursor_y_)
        );
    }

    left_mouse_was_down_ = left_mouse_is_down;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(window_);
}

Window::Window(Window&& other) noexcept
    : window_(std::exchange(other.window_, nullptr))
    , horizontal_resize_cursor_(std::exchange(other.horizontal_resize_cursor_, nullptr))
    , vertical_resize_cursor_(std::exchange(other.vertical_resize_cursor_, nullptr))
    , diagonal_resize_cursor_(std::exchange(other.diagonal_resize_cursor_, nullptr))
    , anti_diagonal_resize_cursor_(
          std::exchange(other.anti_diagonal_resize_cursor_, nullptr))
    , width_(other.width_)
    , height_(other.height_)
{
    if (window_ != nullptr) {
        glfwSetWindowUserPointer(window_, this);
    }
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (window_ != nullptr) {
        if (horizontal_resize_cursor_ != nullptr) glfwDestroyCursor(horizontal_resize_cursor_);
        if (vertical_resize_cursor_ != nullptr) glfwDestroyCursor(vertical_resize_cursor_);
        if (diagonal_resize_cursor_ != nullptr) glfwDestroyCursor(diagonal_resize_cursor_);
        if (anti_diagonal_resize_cursor_ != nullptr) glfwDestroyCursor(anti_diagonal_resize_cursor_);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    window_ = std::exchange(other.window_, nullptr);
    horizontal_resize_cursor_ =
        std::exchange(other.horizontal_resize_cursor_, nullptr);
    vertical_resize_cursor_ =
        std::exchange(other.vertical_resize_cursor_, nullptr);
    diagonal_resize_cursor_ =
        std::exchange(other.diagonal_resize_cursor_, nullptr);
    anti_diagonal_resize_cursor_ =
        std::exchange(other.anti_diagonal_resize_cursor_, nullptr);
    width_ = other.width_;
    height_ = other.height_;

    if (window_ != nullptr) {
        glfwSetWindowUserPointer(window_, this);
    }

    return *this;
}

int Window::GetWidth() const
{
    return width_;
}

int Window::GetHeight() const
{
    return height_;
}

int Window::GetLogicalWidth() const
{
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window_, &width, &height);
    return width;
}

int Window::GetLogicalHeight() const
{
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window_, &width, &height);
    return height;
}

int Window::GetTitleBarHeight() const
{
    constexpr int kDesignTitleBarHeight = 90;
    return static_cast<int>(static_cast<float>(kDesignTitleBarHeight) * kScale);
}

int Window::GetTitleBarControlWidth() const
{
    return GetTitleBarHeight() * 3 / 2;
}

void Window::RenderTitleBar(Renderer& renderer, const Style& style) const
{
    const float window_width = static_cast<float>(GetLogicalWidth());
    const float window_height = static_cast<float>(GetLogicalHeight());
    const float title_bar_height = static_cast<float>(GetTitleBarHeight());
    const float control_width = static_cast<float>(GetTitleBarControlWidth());
    if (window_width <= 0.0f || window_height <= 0.0f) {
        return;
    }

    const auto make_rectangle = [window_width, window_height](
                                    const float left, const float top,
                                    const float right, const float bottom) {
        return PointSet{
            {left / window_width, top / window_height},
            {right / window_width, top / window_height},
            {right / window_width, bottom / window_height},
            {left / window_width, bottom / window_height},
        };
    };

    renderer.RenderPolygon(
        make_rectangle(0.0f, 0.0f, window_width, title_bar_height),
        style.colors.bar);

    const float close_left = window_width - control_width;
    const float maximize_left = window_width - control_width * 2.0f;
    const float minimize_left = window_width - control_width * 3.0f;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window_, &cursor_x, &cursor_y);
    const bool pointer_in_title_bar = cursor_y >= 0.0
        && cursor_y < title_bar_height;
    const bool minimize_hovered = pointer_in_title_bar
        && cursor_x >= minimize_left && cursor_x < maximize_left;
    const bool maximize_hovered = pointer_in_title_bar
        && cursor_x >= maximize_left && cursor_x < close_left;
    const bool close_hovered = pointer_in_title_bar
        && cursor_x >= close_left && cursor_x < window_width;
    renderer.RenderPolygon(
        make_rectangle(minimize_left, 0.0f, minimize_left + control_width,
                       title_bar_height),
        minimize_hovered ? style.colors.hover : style.colors.button);
    renderer.RenderPolygon(
        make_rectangle(maximize_left, 0.0f, close_left, title_bar_height),
        maximize_hovered ? style.colors.hover : style.colors.button);
    renderer.RenderPolygon(
        make_rectangle(close_left, 0.0f, window_width, title_bar_height),
        close_hovered ? style.colors.hover : style.colors.button);

    const float center_y = title_bar_height * 0.5f;
    const float icon_size = title_bar_height * 0.42f;
    renderer.RenderIcon("minus",
                        minimize_left + (control_width - icon_size) * 0.5f,
                        center_y - icon_size * 0.5f,
                        icon_size, icon_size, style.colors.text);
    renderer.RenderIcon("square",
                        maximize_left + (control_width - icon_size) * 0.5f,
                        center_y - icon_size * 0.5f,
                        icon_size, icon_size, style.colors.text);
    renderer.RenderIcon("x",
                        close_left + (control_width - icon_size) * 0.5f,
                        center_y - icon_size * 0.5f,
                        icon_size, icon_size, style.colors.text);
}
