// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/platform/Translator.hpp
// Header for entry translator.


#pragma once

#include <GLFW/glfw3.h>
#include <vector>

#include <self/assets/Icon.hpp>

namespace IconTranslator
{
    GLFWimage ToGlfwImage(const IconImage& image);
    std::vector<GLFWimage> ToGlfwImages(const std::vector<IconImage>& images);
}
