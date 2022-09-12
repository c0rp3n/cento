//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
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

    CENTO_FORCEINLINE friend Rect getRect(const Tile* tile)
    {
        return tile->rect;
    }

    CENTO_FORCEINLINE friend i32 getLeft(const Tile* tile)
    {
        return tile->rect.ll.x;
    }

    CENTO_FORCEINLINE friend i32 getBottom(const Tile* tile)
    {
        return tile->rect.ll.y;
    }

    CENTO_FORCEINLINE friend i32 getRight(const Tile* tile)
    {
        return tile->rect.ur.x;
    }

    CENTO_FORCEINLINE friend i32 getTop(const Tile* tile)
    {
        return tile->rect.ur.y;
    }

    CENTO_FORCEINLINE friend void setRect(Tile* tile, const Rect& r)
    {
        tile->rect = r;
    }

    CENTO_FORCEINLINE friend Stitch& leftBottom(Tile* tile)
    {
        return tile->left;
    }

    CENTO_FORCEINLINE friend Stitch& bottomLeft(Tile* tile)
    {
        return tile->below;
    }

    CENTO_FORCEINLINE friend Stitch& topRight(Tile* tile)
    {
        return tile->right;
    }

    CENTO_FORCEINLINE friend Stitch& rightTop(Tile* tile)
    {
        return tile->above;
    }
};
static_assert(std::is_trivial_v<Tile>);

CENTO_END_NAMESPACE

#endif // centoTile_hpp
