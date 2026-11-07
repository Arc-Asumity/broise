// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/core/WidgetManager.hpp
// Manages registered Widget entries and their created Widgets.

#pragma once

#include <string_view>
#include <vector>

#include <self/modules/IWidgetEntry.hpp>


class WidgetManager
{
public:
    ~WidgetManager();

    WidgetManager() = default;
    WidgetManager(const WidgetManager&) = delete;
    WidgetManager& operator=(const WidgetManager&) = delete;

    void Register(IWidgetEntry& entry);
    [[nodiscard]] IWidget* Create(std::string_view id);
    void Destroy(IWidget& widget) noexcept;

private:
    struct WidgetRecord
    {
        IWidgetEntry* entry;
        IWidget* widget;
    };

    std::vector<IWidgetEntry*> entries_;
    std::vector<WidgetRecord> widgets_;
};
