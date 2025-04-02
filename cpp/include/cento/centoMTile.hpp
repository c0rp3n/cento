//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoZTile_hpp
#define centoZTile_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoRect.hpp"

#include <compare>
#include <typeinfo>

CENTO_BEGIN_NAMESPACE

struct MTile;

using MStitch = MTile*;

/*
 * This is a tile which copies from the Tile Structure type in Magic which only
 * stores the lower left in each tile, it relies on the topRight tile to get the
 * upper right coordinate of a tile.
 *
 *                                    above
 *                                      ^
 *                                      |
 *              +-----------------------+ ---> right
 *              |         (upper right) |
 *              |                       |
 *              |                       |
 *              | (lower left)          |
 *   right <--- +-----------------------+
 *              |
 *              v
 *            below
 *
 */
struct MTile
{
    Point   lowerLeft;
    u64     id;
    MStitch below;
    MStitch left;
    MStitch above;
    MStitch right;

    friend std::strong_ordering operator<=>(const MTile& lhs,
                                            const MTile& rhs) = default;
};
static_assert(std::is_trivial_v<MTile>);
static_assert(sizeof(MTile) == 48);

#pragma pop pack

CENTO_END_NAMESPACE

#endif // centoZTile_hpp
