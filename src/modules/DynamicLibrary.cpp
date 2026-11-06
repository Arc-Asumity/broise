// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/modules/DynamicLibrary.cpp
// Private native dynamic-library wrapper implementation.

#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <internal/modules/DynamicLibrary.hpp>

DynamicLibrary::~DynamicLibrary()
{
    Close();
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr))
{}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept
{
    if (this != &other) {
        Close();
        handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
}

std::expected<DynamicLibrary, std::string>
DynamicLibrary::Open(const std::filesystem::path& path)
{
    DynamicLibrary library;
#ifdef _WIN32
    library.handle_ = LoadLibraryExW(
        path.c_str(), nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (library.handle_ == nullptr) {
        return std::unexpected(
            "LoadLibraryExW failed with error "
            + std::to_string(GetLastError()));
    }
#else
    library.handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (library.handle_ == nullptr) {
        const char* error = dlerror();
        return std::unexpected(error == nullptr ? "dlopen failed" : error);
    }
#endif
    return library;
}

void* DynamicLibrary::FindSymbol(const char* name) const noexcept
{
    if (handle_ == nullptr) {
        return nullptr;
    }
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(
        static_cast<HMODULE>(handle_), name));
#else
    dlerror();
    return dlsym(handle_, name);
#endif
}

void DynamicLibrary::Close() noexcept
{
    if (handle_ == nullptr) {
        return;
    }
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
    handle_ = nullptr;
}
