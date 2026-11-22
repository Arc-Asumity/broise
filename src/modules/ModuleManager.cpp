// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Path /src/modules/ModuleManager.cpp
// Loads native C++ modules from the installed mods directory.

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <self/modules/ModuleManager.hpp>


