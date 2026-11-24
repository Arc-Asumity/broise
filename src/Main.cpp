// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/Main.cpp
// Main of Broise.


#include <filesystem>

#include <self/core/Application.hpp>

int main(const int argc, char* argv[])
{
    const std::filesystem::path installed_directory = argc > 0
        ? std::filesystem::absolute(argv[0]).parent_path()
        : std::filesystem::current_path();
    auto application = Application("Broise", installed_directory);

    application.Run();

    return 0;
}
