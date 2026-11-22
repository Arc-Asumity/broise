// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/module/Preprocess.hpp
// Definition for preprocessing code.


#pragma once

#define BROISE_STRINGIFY_DETAIL(value) #value
#define BROISE_STRINGIFY(value) BROISE_STRINGIFY_DETAIL(value)

#if defined(_WIN32)
#define BROISE_MODULE_EXPORT __declspec(dllexport)
#else
#define BROISE_MODULE_EXPORT __attribute__((visibility("default")))
#endif

#if defined(_MSC_VER)
#define BROISE_ABI_COMPILER "msvc-" BROISE_STRINGIFY(_MSC_VER)
#elif defined(__clang__)
#define BROISE_ABI_COMPILER "clang-" BROISE_STRINGIFY(__clang_major__) "." \
    BROISE_STRINGIFY(__clang_minor__)
#elif defined(__GNUC__)
#define BROISE_ABI_COMPILER "gcc-" BROISE_STRINGIFY(__GNUC__) "." \
    BROISE_STRINGIFY(__GNUC_MINOR__)
#else
#error "Unsupported compiler for the Broise Module ABI"
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define BROISE_ABI_ARCH "x64"
#elif defined(_M_ARM64) || defined(__aarch64__)
#define BROISE_ABI_ARCH "arm64"
#elif defined(_M_IX86) || defined(__i386__)
#define BROISE_ABI_ARCH "x86"
#else
#error "Unsupported architecture for the Broise Module ABI"
#endif

#define BROISE_MODULE_ABI_TAG "broise-module-v1-" BROISE_ABI_COMPILER "-" \
    BROISE_ABI_ARCH
