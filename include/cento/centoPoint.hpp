//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoPoint_hpp
#define centoPoint_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoDefs.hpp"
#include "centoMacros.hpp"

#include <compare>
#include <limits>

CENTO_BEGIN_NAMESPACE

constexpr i32 minx = std::numeric_limits<i32>::min();
constexpr i32 maxx = std::numeric_limits<i32>::max();
constexpr i32 miny = std::numeric_limits<i32>::min();
constexpr i32 maxy = std::numeric_limits<i32>::max();

struct Point
{
    i32 x;
    i32 y;

    friend std::strong_ordering operator<=>(const Point& lhs, const Point& rhs) = default;
};
static_assert(std::is_trivial_v<Point>);

CENTO_END_NAMESPACE

#endif // centoVertex_hpp
