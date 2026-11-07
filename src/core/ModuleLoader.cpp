// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/core/ModuleLoader.cpp
// Loads native C++ modules from the installed mods directory.

#include <algorithm>
#include <filesystem>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <self/core/ModuleLoader.hpp>

namespace
{
    using CreateModuleFunction = Broise::IModule* (*)() noexcept;
    using DestroyModuleFunction = void (*)(const Broise::IModule*) noexcept;

#if defined(_MSC_VER) && defined(_M_X64)
    constexpr char kCreateModuleSymbol[] =
        "?CreateModule@Broise@@YAPEAVIModule@1@XZ";
    constexpr char kDestroyModuleSymbol[] =
        "?DestroyModule@Broise@@YAXPEBVIModule@1@@Z";
#elif defined(__GNUC__) || defined(__clang__)
    constexpr char kCreateModuleSymbol[] = "_ZN6Broise12CreateModuleEv";
    constexpr char kDestroyModuleSymbol[] =
        "_ZN6Broise12DestroyModuleEPKNS_7IModuleE";
#else
#error "Module factory symbols are not defined for this compiler ABI"
#endif

    void* OpenLibrary(const std::filesystem::path& path)
    {
#ifdef _WIN32
        return static_cast<void*>(LoadLibraryW(path.c_str()));
#else
        return dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    }

    void* FindSymbol(void* library, const char* name)
    {
#ifdef _WIN32
        return reinterpret_cast<void*>(GetProcAddress(
            static_cast<HMODULE>(library), name));
#else
        return dlsym(library, name);
#endif
    }

    void CloseLibrary(void* library)
    {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(library));
#else
        dlclose(library);
#endif
    }

    struct LoadedModule
    {
        void* library{};
        Broise::IModule* module{};
        DestroyModuleFunction destroy{};
    };
}

class ModuleLoader::Impl
{
public:
    explicit Impl(const std::filesystem::path& installed_directory)
    {
        const std::filesystem::path modules_directory =
            installed_directory / "mods";
        std::error_code error;
        if (!std::filesystem::is_directory(modules_directory, error) || error) {
            return;
        }

        std::vector<std::filesystem::path> candidates;
        for (std::filesystem::directory_iterator iterator(
                 modules_directory, error), end;
             !error && iterator != end; iterator.increment(error)) {
            if (iterator->is_regular_file(error)
                && !error && iterator->path().extension() == ".dll") {
                candidates.push_back(iterator->path());
            }
        }
        std::sort(candidates.begin(), candidates.end());

        for (const auto& candidate : candidates) {
            Load(candidate);
        }
    }

    ~Impl()
    {
        for (auto iterator = modules_.rbegin(); iterator != modules_.rend();
             ++iterator) {
            LoadedModule& loaded = *iterator;
            if (loaded.module->Shutdown()) {
                loaded.destroy(loaded.module);
                CloseLibrary(loaded.library);
            }
            // Keep the DLL mapped if shutdown fails because module code may
            // still own resources whose destructors live in that DLL.
        }
    }

private:
    void Load(const std::filesystem::path& path)
    {
        void* library = OpenLibrary(path);
        if (library == nullptr) {
            return;
        }

        const auto create = reinterpret_cast<CreateModuleFunction>(
            FindSymbol(library, kCreateModuleSymbol));
        const auto destroy = reinterpret_cast<DestroyModuleFunction>(
            FindSymbol(library, kDestroyModuleSymbol));
        if (create == nullptr || destroy == nullptr) {
            CloseLibrary(library);
            return;
        }

        Broise::IModule* module = create();
        if (module == nullptr) {
            CloseLibrary(library);
            return;
        }
        if (!module->Initialize()) {
            destroy(module);
            CloseLibrary(library);
            return;
        }

        modules_.push_back({library, module, destroy});
    }

    std::vector<LoadedModule> modules_;
};

ModuleLoader::ModuleLoader(std::filesystem::path module_path)
    : impl_(std::make_unique<Impl>(module_path))
{}

ModuleLoader::~ModuleLoader() = default;
ModuleLoader::ModuleLoader(ModuleLoader&&) noexcept = default;
ModuleLoader& ModuleLoader::operator=(ModuleLoader&&) noexcept = default;
