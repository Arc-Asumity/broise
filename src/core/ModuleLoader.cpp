// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/core/ModuleLoader.cpp
// Loads native C++ modules from the installed mods directory.

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string_view>
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

    void* FindExport(void* library, const std::string_view function_name)
    {
#ifdef _WIN32
        const auto* module_base = reinterpret_cast<const std::byte*>(library);
        const auto* dos_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(module_base);
        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
            return nullptr;
        }

        const auto* nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            module_base + dos_header->e_lfanew);
        if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
            return nullptr;
        }

        const IMAGE_DATA_DIRECTORY& export_directory =
            nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (export_directory.VirtualAddress == 0) {
            return nullptr;
        }

        const auto* exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(
            module_base + export_directory.VirtualAddress);
        const auto* name_rvas = reinterpret_cast<const DWORD*>(
            module_base + exports->AddressOfNames);
        for (DWORD index = 0; index < exports->NumberOfNames; ++index) {
            const auto* exported_name = reinterpret_cast<const char*>(
                module_base + name_rvas[index]);
            if (std::string_view{exported_name}.find(function_name)
                != std::string_view::npos) {
                return reinterpret_cast<void*>(GetProcAddress(
                    static_cast<HMODULE>(library), exported_name));
            }
        }
        return nullptr;
#else
        (void)library;
        (void)function_name;
        return nullptr;
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

        modules_.push_back({library, module, destroy});
    }

    std::vector<LoadedModule> modules_;
};

ModuleLoader::ModuleLoader(const std::filesystem::path& module_path)
    : impl_(std::make_unique<Impl>(module_path))
{}

ModuleLoader::~ModuleLoader() = default;
ModuleLoader::ModuleLoader(ModuleLoader&&) noexcept = default;
ModuleLoader& ModuleLoader::operator=(ModuleLoader&&) noexcept = default;
