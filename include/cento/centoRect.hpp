//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoRect_hpp
#define centoRect_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoPoint.hpp"

#include <compare>

CENTO_BEGIN_NAMESPACE

struct Rect
{
    Point ll;
    Point ur;

    friend std::strong_ordering operator<=>(const Rect& lhs, const Rect& rhs) = default;

    CENTO_FORCEINLINE friend Point lowerLeft(const Rect& r)
    {
        return r.ll;
    }

    CENTO_FORCEINLINE friend Point upperRight(const Rect& r)
    {
        return r.ur;
    }
};
static_assert(std::is_trivial_v<Rect>);

CENTO_FORCEINLINE bool contains(const Rect& rect, const Point& point)
{
    return (point.x >= rect.ll.x) && (point.y >= rect.ll.y) &&
           (point.x <= rect.ur.x) && (point.y <= rect.ur.y);
}

CENTO_END_NAMESPACE

#endif // centoRect_hpp
