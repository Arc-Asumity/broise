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
Application::Create(const char* title, IPart& root)
{
    auto window = Window::Create(title);

    if (!window) {
        return std::unexpected(window.error());
    }

    return Application{
        std::move(*window),
        root,
    };
}

Application::Application(Window window, IPart& root)
    : window_(std::move(window))
    , root_(root)
    , render_(Renderer())
{
    render_.Init();
}

void Application::Run()
{
    glClearColor(0, 0, 0, 1);


    while (!window_.ShouldClose()) {
        Window::PollEvents();
        root_.Update();

        glClear(GL_COLOR_BUFFER_BIT);

        root_.Render();

        window_.SwapBuffers();
    }
}

/***
Widget& Application::GetRoot()
{
    return root_;
}

Widget* Application::GetPressedWidget(const Point& point)
{
    return root_.GetPressedWidget(point);
}
***/
