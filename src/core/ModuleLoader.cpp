// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/core/ModuleLoader.cpp
// Loads native C++ modules from the installed mods directory.

#include <algorithm>
#include <filesystem>
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

    void* OpenLibrary(const std::filesystem::path& path)
    {
#ifdef _WIN32
        return static_cast<void*>(LoadLibraryW(path.c_str()));
#else
        return dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    }

    void* FindExport(void* library, const char* name)
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
        std::ranges::sort(candidates);

        for (const auto& candidate : candidates) {
            Load(candidate);
        }
    }

    ~Impl()
    {
        for (auto iterator = modules_.rbegin(); iterator != modules_.rend(); ++iterator)
            if (auto& [library, module, destroy] = *iterator; module->Shutdown()) {
                destroy(module);
                CloseLibrary(library);
            }
    }

private:
    void Load(const std::filesystem::path& path)
    {
        void* library = OpenLibrary(path);
        if (library == nullptr) {
            return;
        }

        void* create_symbol = FindExport(library, "CreateModule");
        void* destroy_symbol = FindExport(library, "DestroyModule");
        const auto create = reinterpret_cast<CreateModuleFunction>(create_symbol);
        const auto destroy = reinterpret_cast<DestroyModuleFunction>(destroy_symbol);
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

        modules_.push_back({.library = library, .module = module, .destroy = destroy});
    }

    std::vector<LoadedModule> modules_;
};

ModuleLoader::ModuleLoader(const std::filesystem::path& module_path)
    : impl_(std::make_unique<Impl>(module_path))
{}

ModuleLoader::~ModuleLoader() = default;
ModuleLoader::ModuleLoader(ModuleLoader&&) noexcept = default;
ModuleLoader& ModuleLoader::operator=(ModuleLoader&&) noexcept = default;
