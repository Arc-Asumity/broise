// ==============================================================
// Copyright (c) 2026 Arc Asumity
// Licensed under the GPLv3 or later.
// https://github.com/Arc-Asumity/broise
// ==============================================================
// Path /include/self/parts/RootPart.hpp
// Header for the root Part.


#pragma once

#include <self/parts/IPart.hpp>

class RootPart : public IPart
{
public:
    RootPart();

    void Render() override;
    void Update() override;
};
