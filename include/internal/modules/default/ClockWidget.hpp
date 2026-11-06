// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Private Clock Widget implementation for DefaultModule.

#pragma once

#include <string>
#include <string_view>

#include <self/parts/IPart.hpp>
#include <self/parts/TextPart.hpp>
#include <self/widgets/IWidget.hpp>

class ClockPart final : public IPart
{
public:
    void SetText(std::string_view text);
    void Render(const PartRenderContext& context) override;
    void Update() override;

private:
    TextPart text_;
    std::string value_;
};

class ClockWidget final : public Broise::IWidget
{
public:
    [[nodiscard]] std::string_view GetName() const noexcept override;
    void Update() override;
    [[nodiscard]] IPart& GetRootPart() noexcept override;

private:
    ClockPart root_part_;
};
