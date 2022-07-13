//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//

#ifndef centoPoint_hpp
#define centoPoint_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoDefs.hpp"

#include <compare>

CENTO_BEGIN_NAMESPACE

struct Point
{
    i32 x;
    i32 y;

    friend std::strong_ordering operator<=>(const Point& lhs, const Point& rhs) = default;
};

CENTO_END_NAMESPACE

#endif // centoVertex_hpp
