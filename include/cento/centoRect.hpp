//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//

#ifndef centoRect_hpp
#define centoRect_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoPoint.hpp"

#include <compare>

CENTO_BEGIN_NAMESPACE

struct Rect
{
    Point ll;
    Point ur;

    friend std::strong_ordering operator<=>(const Rect& lhs, const Rect& rhs) = default;
};

CENTO_END_NAMESPACE

#endif // centoRect_hpp
