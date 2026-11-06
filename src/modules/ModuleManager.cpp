// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/modules/ModuleManager.cpp
// Module discovery, validation, initialization, and shutdown.

#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <internal/modules/DynamicLibrary.hpp>
#include <self/modules/ModuleManager.hpp>

namespace
{
#if defined(_MSC_VER) && defined(_M_X64)
    constexpr char kCreateModuleSymbol[] =
        "?CreateModuleV1@Broise@@YAPEAVIModule@1@XZ";
#elif (defined(__GNUC__) || defined(__clang__)) \
    && defined(__x86_64__) && defined(__APPLE__)
    constexpr char kCreateModuleSymbol[] = "__ZN6Broise14CreateModuleV1Ev";
#elif (defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
    constexpr char kCreateModuleSymbol[] = "_ZN6Broise14CreateModuleV1Ev";
#else
#error "No Module entry-point symbol is defined for this compiler ABI and architecture"
#endif

    using CreateModuleFunction = Broise::IModule* (*)();

    struct ModuleRecord
    {
        DynamicLibrary library;
        Broise::IModule* module{};
        std::filesystem::path path;
    };

    std::vector<ModuleRecord>& PinnedModules()
    {
        // A failed shutdown must keep the module object and its code resident
        // until process exit. Intentionally do not destroy this registry.
        static auto* modules = new std::vector<ModuleRecord>();
        return *modules;
    }

    bool IsValidIdentifier(const std::string_view id)
    {
        if (id.empty()) {
            return false;
        }
        return std::all_of(id.begin(), id.end(), [](const unsigned char ch) {
            return std::isalnum(ch) != 0 || ch == '.' || ch == '_' || ch == '-';
        });
    }

    bool IsDynamicLibrary(const std::filesystem::path& path)
    {
#ifdef _WIN32
        return path.extension() == ".dll";
#elif defined(__APPLE__)
        return path.extension() == ".dylib";
#else
        return path.extension() == ".so";
#endif
    }

}

class Broise::ModuleManager::Impl
{
public:
    ~Impl()
    {
        Shutdown();
    }

    void AddDiagnostic(const std::filesystem::path& path,
                       const ModuleDiagnosticCode code,
                       std::string message)
    {
        diagnostics.push_back({path, code, std::move(message)});
    }

    void LoadCandidate(const std::filesystem::path& path)
    {
        auto library_result = DynamicLibrary::Open(path);
        if (!library_result) {
            AddDiagnostic(path, ModuleDiagnosticCode::LibraryLoadFailed,
                          library_result.error());
            return;
        }

        ModuleRecord record{
            .library = std::move(*library_result),
            .path = path,
        };
        void* symbol = record.library.FindSymbol(kCreateModuleSymbol);
        if (symbol == nullptr) {
            AddDiagnostic(path, ModuleDiagnosticCode::EntryPointNotFound,
                          "The C++ CreateModuleV1 entry point was not found");
            return;
        }

        const auto create_module =
            reinterpret_cast<CreateModuleFunction>(symbol);
        try {
            record.module = create_module();
        } catch (const std::exception& error) {
            AddDiagnostic(path, ModuleDiagnosticCode::FactoryFailed, error.what());
            return;
        } catch (...) {
            AddDiagnostic(path, ModuleDiagnosticCode::FactoryFailed,
                          "The Module factory threw an unknown exception");
            return;
        }

        if (record.module == nullptr) {
            AddDiagnostic(path, ModuleDiagnosticCode::FactoryFailed,
                          "The Module factory returned null");
            return;
        }

        Broise::ModuleInfo module_info{};
        try {
            module_info = record.module->GetInfo();
        } catch (const std::exception& error) {
            AddDiagnostic(path, ModuleDiagnosticCode::InvalidMetadata, error.what());
            record.module->Destroy();
            return;
        } catch (...) {
            AddDiagnostic(path, ModuleDiagnosticCode::InvalidMetadata,
                          "GetInfo threw an unknown exception");
            record.module->Destroy();
            return;
        }

        if (!IsValidIdentifier(module_info.id)
            || module_info.name.empty() || module_info.version.empty()) {
            AddDiagnostic(path, ModuleDiagnosticCode::InvalidMetadata,
                          "Module ID, name, or version is empty or invalid");
            record.module->Destroy();
            return;
        }

        if (module_info.api_version.major != Broise::kModuleApiVersion.major
            || module_info.api_version.minor > Broise::kModuleApiVersion.minor) {
            AddDiagnostic(path, ModuleDiagnosticCode::IncompatibleApi,
                          "Module API version is not compatible with this Runtime");
            record.module->Destroy();
            return;
        }

        if (module_info.pointer_size != sizeof(void*)
            || module_info.abi_tag != Broise::kModuleAbiTag) {
            AddDiagnostic(path, ModuleDiagnosticCode::IncompatibleAbi,
                          "Module compiler, standard library, configuration, or architecture "
                          "does not match this Runtime");
            record.module->Destroy();
            return;
        }

        const std::string module_id{module_info.id};
        if (module_ids.contains(module_id)) {
            AddDiagnostic(path, ModuleDiagnosticCode::DuplicateModuleId,
                          "Another candidate already uses Module ID '"
                              + module_id + "'");
            record.module->Destroy();
            return;
        }

        Broise::LoadedModuleInfo loaded_info{
            .path = path,
            .id = module_id,
            .name = std::string{module_info.name},
            .version = std::string{module_info.version},
            .api_version = module_info.api_version,
        };
        std::set<std::string> widget_ids;
        for (const Broise::WidgetDescriptor& descriptor :
             record.module->GetWidgetDescriptors()) {
            if (!IsValidIdentifier(descriptor.id) || descriptor.name.empty()) {
                AddDiagnostic(path, ModuleDiagnosticCode::InvalidMetadata,
                              "A Widget descriptor has an empty or invalid ID/name");
                record.module->Destroy();
                return;
            }

            std::string widget_id{descriptor.id};
            if (!widget_ids.insert(widget_id).second) {
                AddDiagnostic(path, ModuleDiagnosticCode::DuplicateWidgetId,
                              "Module contains duplicate Widget ID '"
                                  + widget_id + "'");
                record.module->Destroy();
                return;
            }
            loaded_info.widgets.push_back({
                std::move(widget_id),
                std::string{descriptor.name},
                std::string{descriptor.description},
            });
        }

        const Broise::ModuleContext context{};
        bool initialized = false;
        try {
            initialized = record.module->Initialize(context);
        } catch (const std::exception& error) {
            AddDiagnostic(path, ModuleDiagnosticCode::InitializationFailed,
                          error.what());
        } catch (...) {
            AddDiagnostic(path, ModuleDiagnosticCode::InitializationFailed,
                          "Initialize threw an unknown exception");
        }

        if (!initialized) {
            if (diagnostics.empty() || diagnostics.back().path != path
                || diagnostics.back().code != ModuleDiagnosticCode::InitializationFailed) {
                AddDiagnostic(path, ModuleDiagnosticCode::InitializationFailed,
                              "Initialize returned false");
            }
            // A failed Initialize must roll back its own partial state.
            record.module->Destroy();
            return;
        }

        modules.push_back(std::move(record));
        module_ids.insert(module_id);
        loaded_modules.push_back(std::move(loaded_info));
    }

    void DiscoverAndLoad(
        const std::span<const std::filesystem::path> directories)
    {
        std::vector<std::filesystem::path> candidates;
        std::set<std::filesystem::path> unique_paths;

        for (const std::filesystem::path& directory : directories) {
            std::error_code error;
            if (!std::filesystem::exists(directory, error) || error
                || !std::filesystem::is_directory(directory, error) || error) {
                AddDiagnostic(directory, ModuleDiagnosticCode::DirectoryScanFailed,
                              error ? error.message() : "Directory does not exist");
                continue;
            }

            std::filesystem::recursive_directory_iterator iterator(
                directory, std::filesystem::directory_options::skip_permission_denied,
                error);
            const std::filesystem::recursive_directory_iterator end;
            if (error) {
                AddDiagnostic(directory, ModuleDiagnosticCode::DirectoryScanFailed,
                              error.message());
                continue;
            }

            while (iterator != end) {
                std::error_code file_error;
                if (iterator->is_regular_file(file_error) && !file_error
                    && IsDynamicLibrary(iterator->path())) {
                    std::error_code canonical_error;
                    const auto canonical_path = std::filesystem::weakly_canonical(
                        iterator->path(), canonical_error);
                    const auto& candidate = canonical_error
                        ? iterator->path() : canonical_path;
                    if (unique_paths.insert(candidate).second) {
                        candidates.push_back(candidate);
                    }
                }

                iterator.increment(error);
                if (error) {
                    AddDiagnostic(directory,
                                  ModuleDiagnosticCode::DirectoryScanFailed,
                                  error.message());
                    break;
                }
            }
        }

        std::sort(candidates.begin(), candidates.end());
        for (const std::filesystem::path& candidate : candidates) {
            LoadCandidate(candidate);
        }
    }

    void Shutdown() noexcept
    {
        if (shutdown) {
            return;
        }
        shutdown = true;

        for (ModuleRecord& record : modules) {
            bool stopped = false;
            try {
                stopped = record.module->Shutdown();
            } catch (...) {
                stopped = false;
            }

            if (!stopped) {
                diagnostics.push_back({
                    record.path,
                    ModuleDiagnosticCode::ShutdownFailed,
                    "Module shutdown failed; its library remains loaded until process exit",
                });
                PinnedModules().push_back(std::move(record));
                continue;
            }

            record.module->Destroy();
            record.module = nullptr;
            record.library.Close();
        }
        modules.clear();
        loaded_modules.clear();
        module_ids.clear();
    }

    std::vector<ModuleRecord> modules;
    std::vector<Broise::LoadedModuleInfo> loaded_modules;
    std::vector<Broise::ModuleDiagnostic> diagnostics;
    std::set<std::string> module_ids;
    bool shutdown{};
};

Broise::ModuleManager::ModuleManager()
    : impl_(std::make_unique<Impl>())
{}

Broise::ModuleManager::~ModuleManager() = default;

Broise::ModuleManager::ModuleManager(ModuleManager&&) noexcept = default;

Broise::ModuleManager& Broise::ModuleManager::operator=(ModuleManager&&) noexcept = default;

void Broise::ModuleManager::DiscoverAndLoad(
    const std::span<const std::filesystem::path> directories)
{
    if (impl_) {
        impl_->DiscoverAndLoad(directories);
    }
}

std::span<const Broise::LoadedModuleInfo>
Broise::ModuleManager::GetLoadedModules() const noexcept
{
    if (!impl_) {
        return {};
    }
    return impl_->loaded_modules;
}

std::span<const Broise::ModuleDiagnostic>
Broise::ModuleManager::GetDiagnostics() const noexcept
{
    if (!impl_) {
        return {};
    }
    return impl_->diagnostics;
}

void Broise::ModuleManager::Shutdown() noexcept
{
    if (impl_) {
        impl_->Shutdown();
    }
}
