// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/Translator.cpp
// Translate item for GLFW.


#include <self/platform/Translator.hpp>

using WindowGlfwImages = std::array<GLFWimage, kWindowIconSize.size()>;

GLFWimage IconTranslator::ToGlfwImage(const IconImage& image)
{
    return GLFWimage{
        .width = image.width,
        .height = image.height,
        .pixels = const_cast<unsigned char*>(image.pixels.data())
    };
}

WindowGlfwImages IconTranslator::ToGlfwImages(const WindowIconImages& images)
{
    WindowGlfwImages result;

    for (size_t i = 0; i < kWindowIconSize.size(); i++)
        result[i] = ToGlfwImage(images[i]);

    return result;
}
