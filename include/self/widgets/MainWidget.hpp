// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/widgets/MainWidget.hpp
// Header for main widget.


#pragma once

#include <self/core/IWidget.hpp>

class MainWidget : public IWidget
{
public:
    MainWidget();

    void Render() override;
    void Update() override;
};
