//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoSplit_hpp
#define centoSplit_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoPlane.hpp"

CENTO_BEGIN_NAMESPACE

struct HorzSplit
{
    Tile* upper;
    Tile* lower;

    CENTO_FORCEINLINE explicit operator bool() const noexcept
    {
        return upper != nullptr;
    }
};

CENTO_FORCEINLINE HorzSplit splitTileHorz(Plane& plane, Tile* tile, const i32 y)
{
    const Rect r = getRect(tile);
    if (lowerLeft(r).y >= y) { return {}; }
    if (upperRight(r).y <= y) { return {}; }

    Tile* const upper = plane.allocator.get();
    Tile* const lower = tile;

    upper->id = Space;

    // update rects
    setRect(upper, Rect{.ll = {r.ll.x, y}, .ur = r.ur});
    setRect(lower, Rect{.ll = r.ll, .ur = {r.ur.x, y}});

    // update links
    leftBottom(upper) = lower;
    rightTop(upper)   = rightTop(lower);
    topRight(upper)   = topRight(lower);

    Tile* tp = nullptr;

    // adjust corner stitches along top edge
    for (tp = rightTop(lower); tp && leftBottom(tp) == lower; tp = bottomLeft(tp))
        leftBottom(tp) = upper;
    rightTop(lower) = upper;

    // adjust corner stitches along right edge
    for (tp = topRight(lower); tp && getBottom(tp) >= y; tp = leftBottom(tp))
        bottomLeft(tp) = upper;
    topRight(lower) = tp;

    // adjust corner stitches along left edge
    for (tp = bottomLeft(lower); tp && getTop(tp) <= y; tp = rightTop(tp))
        /* nothing */;

    bottomLeft(upper) = tp;
    while (tp && topRight(tp) == lower)
    {
        topRight(tp) = upper;
        tp = rightTop(tp);
    }

    return HorzSplit{.upper = upper, .lower = lower};
}

struct VertSplit
{
    Tile* left;
    Tile* right;

    CENTO_FORCEINLINE explicit operator bool() const noexcept
    {
        return left != nullptr;
    }
};

CENTO_FORCEINLINE VertSplit splitTileVert(Plane& plane, Tile* tile, const i32 x)
{
    const Rect r = getRect(tile);
    if (lowerLeft(r).x >= x) { return {}; }
    if (upperRight(r).x <= x) { return {}; }

    Tile* const left  = tile;
    Tile* const right = plane.allocator.get();

    // update body
    right->id = Space;

    // update rects
    setRect(left, Rect{.ll = r.ll, .ur = {.x = x, .y = r.ur.y}});
    setRect(right, Rect{.ll = {.x = x, .y = r.ll.y}, .ur = r.ur});

    // update stiches
    bottomLeft(right) = left;
    topRight(right) = topRight(left);
    rightTop(right) = rightTop(left);

    Tile* tp = nullptr;

    // adjust corner stitches along the right edge
    for (tp = topRight(left); tp && bottomLeft(tp) == left; tp = leftBottom(tp))
        bottomLeft(tp) = right;
    topRight(left) = right;

    // adjust corner stitches along the top edge
    for (tp = rightTop(left); tp && getLeft(tp) >= x; tp = bottomLeft(tp))
        leftBottom(tp) = right;
    rightTop(left) = tp;

    // adjust corner stitches along the bottom edge
    for (tp = leftBottom(left); tp && getRight(tp) <= x; tp = topRight(tp))
        /* nothing */;

    leftBottom(right) = tp;
    while (tp && rightTop(tp) == left)
    {
        rightTop(tp) = right;
        tp = topRight(tp);
    }

    return VertSplit{.left = left, .right = right};
}

CENTO_FORCEINLINE Tile* splitTile(Plane& plane, Tile* tile, const Rect& r)
{
    const i32 LL_x = r.ll.x;
    const i32 LL_y = r.ll.y;
    const i32 UR_x = r.ur.x;
    const i32 UR_y = r.ur.y;

    if (const HorzSplit split = splitTileHorz(plane, tile, UR_y); split)
    {
        tile = split.lower;
    }
    if (const HorzSplit split = splitTileHorz(plane, tile, LL_y); split)
    {
        tile = split.upper;
    }
    if (const VertSplit split = splitTileVert(plane, tile, UR_x); split)
    {
        tile = split.left;
    }
    if (const VertSplit split = splitTileVert(plane, tile, LL_x); split)
    {
        tile = split.right;
    }

    return tile;
}

CENTO_END_NAMESPACE

#endif // centoSplit_hpp
