//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoTile_hpp
#define centoTile_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoRect.hpp"

#include <compare>
#include <typeinfo>
#include <type_traits>

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

    ////////////////////////////////////////////////////////////////////////////
    // Rectangle
    ////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////
    // Body
    ////////////////////////////////////////////////////////////////////////////

    CENTO_FORCEINLINE friend bool isSolid(const Tile* tile)
    {
        return tile->id != Space;
    }

    CENTO_FORCEINLINE friend bool isSpace(const Tile* tile)
    {
        return tile->id == Space;
    }

    CENTO_FORCEINLINE friend void setBody(Tile* tile, const u64 id)
    {
        tile->id = id;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Stitches
    ////////////////////////////////////////////////////////////////////////////

    CENTO_FORCEINLINE friend Stitch& bottomLeft(Tile* tile)
    {
        return tile->left;
    }

    CENTO_FORCEINLINE friend Stitch bottomLeft(const Tile* tile)
    {
        return tile->left;
    }

    CENTO_FORCEINLINE friend Stitch& leftBottom(Tile* tile)
    {
        return tile->below;
    }

    CENTO_FORCEINLINE friend Stitch leftBottom(const Tile* tile)
    {
        return tile->below;
    }

    CENTO_FORCEINLINE friend Stitch& topRight(Tile* tile)
    {
        return tile->right;
    }

    CENTO_FORCEINLINE friend Stitch topRight(const Tile* tile)
    {
        return tile->right;
    }

    CENTO_FORCEINLINE friend Stitch& rightTop(Tile* tile)
    {
        return tile->above;
    }

    CENTO_FORCEINLINE friend Stitch rightTop(const Tile* tile)
    {
        return tile->above;
    }

    CENTO_FORCEINLINE friend Stitch leftTop(const Tile* tile)
    {
        const i32 x = getLeft(tile);

        Tile* above = topRight(tile);
        while(above && getRight(above) > x) { above = leftBottom(above); }

        return above;
    }

    CENTO_FORCEINLINE friend Stitch topLeft(const Tile* tile)
    {
        const i32 y = getTop(tile);

        Tile* left = leftBottom(tile);
        while(left && getBottom(left) < y) { left = topRight(left); }

        return left;
    }

    CENTO_FORCEINLINE friend Stitch rightBottom(const Tile* tile)
    {
        const i32 x = getRight(tile);

        Tile* bottom = bottomLeft(tile);
        while(bottom && getLeft(bottom) < x) { bottom = rightTop(bottom); }

        return bottom;
    }

    CENTO_FORCEINLINE friend Stitch bottomRight(const Tile* tile)
    {
        const i32 y = getBottom(tile);

        Tile* right = rightTop(tile);
        while(right && getTop(right) < y) { right = bottomLeft(right); }

        return right;
    }
};
static_assert(std::is_trivial_v<Tile>);

CENTO_END_NAMESPACE

#endif // centoTile_hpp
