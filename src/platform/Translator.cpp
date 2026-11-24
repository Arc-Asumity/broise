// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/platform/Translator.cpp
// Translate item for GLFW.


#include <self/platform/Translator.hpp>

GLFWimage IconTranslator::ToGlfwImage(const IconImage& image)
{
    return GLFWimage{
        .width = image.width,
        .height = image.height,
        .pixels = const_cast<unsigned char*>(image.pixels.data())
    };
}

std::vector<GLFWimage> IconTranslator::ToGlfwImages(const std::vector<IconImage>& images)
{
    std::vector<GLFWimage> result;
    result.reserve(images.size());

    for (const IconImage& image : images)
        result.push_back(ToGlfwImage(image));

    return result;
}
