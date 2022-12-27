//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoRemove_hpp
#define centoRemove_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoExplore.hpp"
#include "centoMerge.hpp"
#include "centoPlane.hpp"
#include "centoSplit.hpp"

#include <algorithm>
#include <gsl/assert>
#include <vector>

CENTO_BEGIN_NAMESPACE

namespace detail
{

    template <typename F> requires std::invocable<F&&, Tile*>
    CENTO_FORCEINLINE Tile* safeIterate(Tile*       tile,
                                        F&&         callback,
                                        auto&&      stopPred,
                                        auto&&      nextTile)
    {
        do
        {
            Tile* next = nextTile(tile);

            callback(tile);

            tile = next;
        } while (stopPred(tile));

        return tile;
    }

    template <typename F> requires std::invocable<F&&, Tile*>
    CENTO_FORCEINLINE Tile* iterDown(Tile*       tile,
                                     i32         to,
                                     F&&         callback)
    {
        auto stop = [=](Tile* t){ return getBottom(t) >= to; };
        auto next = [](Tile* t){ return leftBottom(t); };

        return safeIterate(tile, std::forward<F>(callback), stop, next);
    }

    template <typename F> requires std::invocable<F&&, Tile*>
    CENTO_FORCEINLINE Tile* iterUp(Tile*       tile,
                                   i32         to,
                                   F&&         callback)
    {
        auto stop = [=](Tile* t){ return getTop(t) <= to; };
        auto next = [](Tile* t){ return rightTop(t); };

        return safeIterate(tile, std::forward<F>(callback), stop, next);
    }

}

CENTO_FORCEINLINE void removeTile(Plane& plane, Tile* tile)
{
    Expects(not isSpace(tile));

    // 1. Change the type of the deleted tile from solid to space.
    tile->id = cento::Space;

    const Rect box       = getRect(tile);
    const i32  delBottom = box.ll.y;
    const i32  delTop    = box.ur.y;

    Tile* leftStart  = bottomLeft(tile);
    Tile* rightStart = topRight(tile);

    // 2. If the top right tile is taller than the delete tile we should split
    //    it so we can merge with it.
    if (isSpace(rightStart) && (getTop(rightStart) > delTop))
    {
        rightStart = splitTileHorz(plane, rightStart, delTop).lower;
    }

    // 3. Loop through the tiles along the right edge, split the deleted tile
    //    along the bottom edge of the right tile.
    //    Then look to merge the newly split tile to the right.
    auto rSplitMerge = [&](Tile* t)
    {
        Tile* upper = tile;
        if (const HorzSplit split = splitTileHorz(plane, tile, getBottom(t)); split)
        {
            upper = split.upper;
        }

        if (isSolid(t)) { return; }

        mergeRight(plane, upper);
    };
    Tile* rb = detail::iterDown(rightStart, delBottom, rSplitMerge);

    // 4. If the last tile along the right edge of the deleted tile is taller
    //    than the bottom of the deleted tile, split it and then look to merge
    //    it with the delete tile.
    if (isSpace(rb) && (getTop(rb) > delBottom))
    {
        rb           = splitTileHorz(plane, rb, delBottom).upper;
        Tile* merged = mergeLeft(plane, rb);
        if (merged) { rb = merged; }
    }

    // 5. Check if the bottom right tile can now be merged upwards.
    if (isSolid(bottomLeft(rightTop(rb)))) { mergeUp(plane, rb); }

    // 6. If the lower left tile is lower than the bottom of the deleted tile
    //    we should split them so they share a common bottom edge.
    if (isSpace(leftStart) && (getBottom(leftStart) < delBottom))
    {
        leftStart = splitTileHorz(plane, leftStart, delBottom).upper;
    }

    // 7. Loop through all of the left tiles of the deleted tile and split the
    //    deleted tile wherever there is not already a edge for the top edge.
    //    Then also split the left tile wherever there is a split in the deleted
    //    tile from the right tiles.
    auto lSplit = [&](Tile* t)
    {
        Tile* right = topRight(t);
        if (getTop(t) < getTop(right))
        {
            splitTileHorz(plane, right, getTop(t));
        }

        if (isSolid(t)) { return; }

        for (right = topRight(t); getBottom(t) < getBottom(right); right = leftBottom(right))
        {
            t = splitTileHorz(plane, t, getBottom(right)).lower;
        }
    };
    detail::iterUp(leftStart, delTop, lSplit);

    // 8. Now all of the left tiles have been split look to merge them to the
    //    right with the deleted tile and then down with any other space tiles.
    auto lMerge = [&](Tile* t)
    {
        mergeRight(plane, t);
        mergeDown(plane, t);
    };
    Tile* lt = detail::iterUp(leftStart, delTop, lMerge);

    // 9. If the left top tile is taller than the deleted tile we should split
    //    it so they share a common edge and then try and merge it.
    if (isSpace(lt) && (getBottom(lt) < delTop))
    {
        const HorzSplit split = splitTileHorz(plane, lt, delTop);
        mergeRight(plane, lt);
        lt = split.upper;
    }

    // 10. Finally try to merge the left top tile down.
    mergeDown(plane, lt);
}

CENTO_END_NAMESPACE

#endif // centoRemove_hpp
