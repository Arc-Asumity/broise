// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/Main.cpp
// Main of Broise.

#include <exception>
#include <filesystem>
#include <iostream>

#include <self/core/Application.hpp>

int main(const int argc, char* argv[])
{
    const std::filesystem::path installed_directory = argc > 0
        ? std::filesystem::absolute(argv[0]).parent_path()
        : std::filesystem::current_path();
    auto application = Application::Create("Broise", installed_directory);
    if (!application) {
        std::cerr << "Failed to create application (error code "
                  << static_cast<int>(application.error()) << ").\n";
        return 1;
    }

    try {
        application->Run();
    } catch (const std::exception& error) {
        std::cerr << "Application failed: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
