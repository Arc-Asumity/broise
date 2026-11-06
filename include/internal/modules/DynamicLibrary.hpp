// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/internal/modules/DynamicLibrary.hpp
// Private native dynamic-library wrapper.

#pragma once

#include <expected>
#include <filesystem>
#include <string>

class DynamicLibrary
{
public:
    DynamicLibrary() = default;
    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    [[nodiscard]] static std::expected<DynamicLibrary, std::string>
    Open(const std::filesystem::path& path);

    [[nodiscard]] void* FindSymbol(const char* name) const noexcept;
    void Close() noexcept;

private:
#ifdef _WIN32
    void* handle_{};
#else
    void* handle_{};
#endif
};
