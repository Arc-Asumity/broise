// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /src/core/WidgetManager.cpp
// Widget entry registration and Widget lifetime management.

#include <algorithm>

#include <self/core/WidgetManager.hpp>

namespace Broise
{
    WidgetManager::~WidgetManager()
    {
        for (const WidgetRecord& record : widgets_) {
            record.entry->Destroy(record.widget);
        }
    }

    void WidgetManager::Register(IWidgetEntry& entry)
    {
        if (std::find(entries_.begin(), entries_.end(), &entry) == entries_.end()) {
            entries_.push_back(&entry);
        }
    }

    IWidget* WidgetManager::Create(const std::string_view id)
    {
        const auto entry = std::find_if(
            entries_.begin(), entries_.end(), [id](const IWidgetEntry* candidate) {
                return candidate->GetId() == id;
            });
        if (entry == entries_.end()) {
            return nullptr;
        }

        IWidget* widget = (*entry)->Create();
        if (widget != nullptr) {
            widgets_.push_back({*entry, widget});
        }
        return widget;
    }

    void WidgetManager::Destroy(IWidget& widget) noexcept
    {
        const auto record = std::find_if(
            widgets_.begin(), widgets_.end(), [&widget](const WidgetRecord& candidate) {
                return candidate.widget == &widget;
            });
        if (record == widgets_.end()) {
            return;
        }

        record->entry->Destroy(record->widget);
        widgets_.erase(record);
    }
}
