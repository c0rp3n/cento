//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoTilePlan_hpp
#define centoTilePlan_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoRect.hpp"

#include <compare>
#include <typeinfo>

CENTO_BEGIN_NAMESPACE

struct TilePlan
{
    u64    id;
    Rect   rect;

    friend std::strong_ordering operator<=>(const TilePlan& lhs, const TilePlan& rhs) = default;
};
static_assert(std::is_trivial_v<TilePlan>);

CENTO_END_NAMESPACE

#endif // centoTilePlan_hpp
