// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/module/Preprocess.hpp
// Definition for preprocessing code.


#pragma once

#if defined(_WIN32)
#define BROISE_MODULE_EXPORT __declspec(dllexport)
#else
#define BROISE_MODULE_EXPORT __attribute__((visibility("default")))
#endif
