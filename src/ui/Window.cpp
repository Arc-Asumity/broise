// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Window.cpp
// Window for Broise.


#include <algorithm>
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
    if (left_mouse_is_down && !left_mouse_was_down_ && cursor_y < title_bar_height) {
        if (cursor_x >= window_width - control_width) {
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        } else if (cursor_x >= window_width - 2.0 * control_width) {
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
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    window_ = std::exchange(other.window_, nullptr);
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
    return std::clamp(GetLogicalHeight() / 18, 36, 80);
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
    const float minimize_left = window_width - control_width * 2.0f;
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window_, &cursor_x, &cursor_y);
    const bool pointer_in_title_bar = cursor_y >= 0.0
        && cursor_y < title_bar_height;
    const bool minimize_hovered = pointer_in_title_bar
        && cursor_x >= minimize_left && cursor_x < close_left;
    const bool close_hovered = pointer_in_title_bar
        && cursor_x >= close_left && cursor_x < window_width;
    renderer.RenderPolygon(
        make_rectangle(minimize_left, 0.0f, minimize_left + control_width,
                       title_bar_height),
        minimize_hovered ? style.colors.hover : style.colors.button);
    renderer.RenderPolygon(
        make_rectangle(close_left, 0.0f, window_width, title_bar_height),
        close_hovered ? style.colors.hover : style.colors.button);

    const float center_y = title_bar_height * 0.5f;
    const float icon_size = title_bar_height * 0.42f;
    renderer.RenderIcon("minus",
                        minimize_left + (control_width - icon_size) * 0.5f,
                        center_y - icon_size * 0.5f,
                        icon_size, icon_size, style.colors.text);
    renderer.RenderIcon("x",
                        close_left + (control_width - icon_size) * 0.5f,
                        center_y - icon_size * 0.5f,
                        icon_size, icon_size, style.colors.text);
}
