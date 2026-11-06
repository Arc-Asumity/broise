// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/modules/IModule.hpp
// Public interface for an application or feature Module.

#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include <self/modules/IWidgetEntry.hpp>

namespace Broise
{
    struct ApiVersion
    {
        std::uint32_t major;
        std::uint32_t minor;
    };

    inline constexpr ApiVersion kModuleApiVersion{.major = 2, .minor = 0};

#define BROISE_MODULE_STRINGIFY_IMPL(value) #value
#define BROISE_MODULE_STRINGIFY(value) BROISE_MODULE_STRINGIFY_IMPL(value)

#if defined(_MSC_VER)
#define BROISE_MODULE_COMPILER_TAG \
    "msvc-" BROISE_MODULE_STRINGIFY(_MSC_VER)
#elif defined(__clang__)
#define BROISE_MODULE_COMPILER_TAG \
    "clang-" BROISE_MODULE_STRINGIFY(__clang_major__) "." \
        BROISE_MODULE_STRINGIFY(__clang_minor__) "." \
            BROISE_MODULE_STRINGIFY(__clang_patchlevel__)
#elif defined(__GNUC__)
#define BROISE_MODULE_COMPILER_TAG \
    "gcc-" BROISE_MODULE_STRINGIFY(__GNUC__) "." \
        BROISE_MODULE_STRINGIFY(__GNUC_MINOR__) "." \
            BROISE_MODULE_STRINGIFY(__GNUC_PATCHLEVEL__)
#else
#error "Unsupported compiler for the Broise C++ Module ABI"
#endif

#if defined(_LIBCPP_VERSION)
#define BROISE_MODULE_STDLIB_TAG \
    "libcxx-" BROISE_MODULE_STRINGIFY(_LIBCPP_VERSION)
#elif defined(_GLIBCXX_RELEASE)
#define BROISE_MODULE_STDLIB_TAG \
    "libstdcxx-" BROISE_MODULE_STRINGIFY(_GLIBCXX_RELEASE) \
        "-abi" BROISE_MODULE_STRINGIFY(_GLIBCXX_USE_CXX11_ABI)
#elif defined(_MSVC_STL_VERSION)
#define BROISE_MODULE_STDLIB_TAG \
    "msvcstl-" BROISE_MODULE_STRINGIFY(_MSVC_STL_VERSION) \
        "-iter" BROISE_MODULE_STRINGIFY(_ITERATOR_DEBUG_LEVEL)
#else
#define BROISE_MODULE_STDLIB_TAG "unknown-stdlib"
#endif

#if defined(NDEBUG)
#define BROISE_MODULE_CONFIG_TAG "release"
#else
#define BROISE_MODULE_CONFIG_TAG "debug"
#endif

    inline constexpr std::string_view kModuleAbiTag =
        BROISE_MODULE_COMPILER_TAG "+" BROISE_MODULE_STDLIB_TAG "+"
        BROISE_MODULE_CONFIG_TAG;

#undef BROISE_MODULE_CONFIG_TAG
#undef BROISE_MODULE_STDLIB_TAG
#undef BROISE_MODULE_COMPILER_TAG
#undef BROISE_MODULE_STRINGIFY
#undef BROISE_MODULE_STRINGIFY_IMPL

    struct ModuleInfo
    {
        std::string_view id;
        std::string_view name;
        std::string_view version;
        ApiVersion api_version;
        std::uint32_t pointer_size;
        std::string_view abi_tag;
    };

    struct ModuleContext
    {
        ApiVersion runtime_api_version{kModuleApiVersion};
    };

    class IModule
    {
    public:
        virtual ~IModule() = default;

        [[nodiscard]] virtual ModuleInfo GetInfo() const = 0;
        [[nodiscard]] virtual std::span<IWidgetEntry* const>
        GetWidgetEntries() const noexcept = 0;

        virtual bool Initialize(const ModuleContext& context) = 0;

        virtual bool Shutdown() noexcept = 0;

        virtual void Destroy() noexcept = 0;
    };
}

#if defined(_WIN32)
#if defined(BROISE_MODULE_BUILD)
#define BROISE_MODULE_EXPORT __declspec(dllexport)
#else
#define BROISE_MODULE_EXPORT
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define BROISE_MODULE_EXPORT __attribute__((visibility("default")))
#else
#define BROISE_MODULE_EXPORT
#endif

namespace Broise
{
    BROISE_MODULE_EXPORT IModule* CreateModuleV1();
}
