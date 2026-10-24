// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/Application.cpp
// Application for software.


#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <self/core/Application.hpp>

std::expected<Application, WindowError>
Application::Create(const char* title)
{
    auto window = Window::Create(title);

    if (!window) {
        return std::unexpected(window.error());
    }

    return Application{
        std::move(*window),
    };
}

Application::Application(Window window)
    : window_(std::move(window))
    , render_(Renderer())
{
    render_.Init();
}

void Application::Run()
{
    glClearColor(0, 0, 0, 1);

    root_ = MainWidget::MakeMain(window_.GetHeight());
    MainWidget::LoadSub(
        root_,
        MakeRectangle(Color{.r = 0.1f, .g = 0.1f, .b = 0.1f, .a = 1.0f}),
        MakeRectangle(Color{.r = 0.15f, .g = 0.15f, .b = 0.15f, .a = 1.0f})
    );

    window_.LoadRoot(&root_);

    while (!window_.ShouldClose()) {
        Window::PollEvents();

        if (window_.IsMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            if (!is_press_) {
                is_press_ = true;
                press_position_ = window_.GetMousePosition();
                GetPressedWidget(press_position_)->HandlePressed(press_position_);
            }
        } else
        {
            if (is_press_)
            {
                is_press_ = false;
                // TODO(press)
                std::cout << "Click From " << press_position_ << " To " << window_.GetMousePosition() << std::endl;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        root_.Render(render_);

        window_.SwapBuffers();
    }
}

Widget& Application::GetRoot()
{
    return root_;
}

Widget* Application::GetPressedWidget(const Point& point)
{
    return root_.GetPressedWidget(point);
}

