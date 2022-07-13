//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//

#ifndef centoTile_hpp
#define centoTile_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoRect.hpp"

#include <compare>
#include <typeinfo>

CENTO_BEGIN_NAMESPACE

struct Tile;

using Stitch = Tile*;

constexpr const u64 Space = u64(-1);

/*
 * A tile is the basic unit used for representing both space and
 * solid area in a plane.  It has the following structure:
 *
 *                                             RT
 *                                      ^
 *                                      |
 *              +-----------------------+ ---> TR
 *              |                       |
 *              |                       |
 *              |                       |
 *              | (lower left)          |
 *      BL <--- +-----------------------+
 *              |
 *              v
 *              LB
 *
 * The (x, y) coordinates of the lower left corner of the tile are stored,
 * along with four "corner stitches": RT, TR, BL, LB.
 *
 * Space tiles are distinguished at a higher level by having a distinguished
 * tile body.
 */
struct Tile
{
    Rect   rect;
    u64    id;
    Stitch below;
    Stitch left;
    Stitch above;
    Stitch right;

    friend std::strong_ordering operator<=>(const Tile& lhs, const Tile& rhs) = default;
};
static_assert(std::is_trivial_v<Tile>);

CENTO_END_NAMESPACE

#endif // centoTile_hpp