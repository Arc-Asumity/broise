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
#include <self/ui/Layout.hpp>


class WidgetManager
{
public:
    ~WidgetManager();

    WidgetManager() = default;
    WidgetManager(const WidgetManager&) = delete;
    WidgetManager& operator=(const WidgetManager&) = delete;
    WidgetManager(WidgetManager&&) noexcept = default;
    WidgetManager& operator=(WidgetManager&&) noexcept = default;

    void Register(Broise::IWidgetEntry& entry);
    [[nodiscard]] Broise::IWidget* Create(std::string_view id);
    void Destroy(Broise::IWidget& widget) noexcept;

private:
    Layout layout_;
    struct WidgetRecord
    {
        Broise::IWidgetEntry* entry;
        Broise::IWidget* widget;
    };

    std::vector<Broise::IWidgetEntry*> entries_;
    std::vector<WidgetRecord> widgets_;
};
