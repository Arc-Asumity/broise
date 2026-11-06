// ============================================================== 
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ============================================================== 
// Clock Widget implementation.

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

#include <internal/modules/default/ClockWidget.hpp>

void ClockPart::SetText(const std::string_view text)
{
    value_ = text;
}

void ClockPart::Render(const PartRenderContext& context)
{
    const auto font = context.style.fonts.find("common");
    if (font == context.style.fonts.end()
        || context.logical_width <= 0.0f
        || context.title_bar_height <= 0.0f) {
        return;
    }

    FontStyle scaled_font = font->second;
    scaled_font.size *= context.title_bar_height / 32.0f;
    const float text_width = scaled_font.size * 6.0f;
    const float x = context.logical_width
                    - 2.0f * context.title_bar_control_width
                    - text_width - 8.0f;
    const float y = (context.title_bar_height - scaled_font.size) / 2.0f;

    text_.SetText(value_);
    text_.SetFontStyle(std::move(scaled_font));
    text_.SetBounds({x, y, text_width, context.title_bar_height});
    text_.SetColor(context.style.title_bar_icon_color);
    text_.Render(context);
}

void ClockPart::Update() {}

std::string_view ClockWidget::GetName() const noexcept
{
    return "Clock";
}

void ClockWidget::Update()
{
    const std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::tm local_time{};
#ifdef _WIN32
    localtime_s(&local_time, &current_time);
#else
    localtime_r(&current_time, &local_time);
#endif

    std::ostringstream formatted_time;
    formatted_time << std::put_time(&local_time, "%H:%M:%S");
    root_part_.SetText(formatted_time.str());
}

IPart& ClockWidget::GetRootPart() noexcept
{
    return root_part_;
}
