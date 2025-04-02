//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoInsert_hpp
#define centoInsert_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoExplore.hpp"
#include "centoMerge.hpp"
#include "centoPlane.hpp"
#include "centoSplit.hpp"

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE Tile* insertTile(Plane& plane, const TilePlan& plan)
{
    if (not empty(plane, plan.rect)) { return nullptr; }

    // 1. Find the space tile containing the top edge of the area to be occupied
    //    by the new tile (because of the strip property, a single space tile
    //    must contain the entire edge).
    Tile* t = findTileAt(plane, plan.rect.ur);

    // 2. Split the top space tile along a horizontal line into a piece entirely
    //    above the new tile and a piece overlapping the new tile. Update corner
    //    stitches in the tiles adjoining the new tile.
    splitTileHorz(plane, t, plan.rect.ur.y);

    // 3. Find the space tile containing the bottom edge of the new solid tile,
    //    split it in the same fashion, and update the stitches around it.
    t = findTileAt(plane, plan.rect.ll);
    const HorzSplit split = splitTileHorz(plane, t, plan.rect.ll.y);

    Tile* ret = split ? split.upper : t;

    // 4. Work up the sides of the new tile. Each space tile must be split into
    //    two space tiles, one to the left of the new solid tile and one to the
    //    right. This splitting may make it possible to merge space tiles
    //    vertically: merge whenever possible. After each split or merge,
    //    stitches must be updated in adjoining tiles.
    detail::iterateTiles(ret, [&](Tile* t) -> Tile*
    {
        Tile* tile = t;
        if (const VertSplit split = splitTileVert(plane, tile, plan.rect.ll.x); split)
        {
            mergeDown(plane, split.left);
            tile = split.right;
        }
        if (const VertSplit split = splitTileVert(plane, tile, plan.rect.ur.x); split)
        {
            tile = split.left;
            mergeDown(plane, split.right);
        }

        if (Tile* const merge = mergeDown(plane, tile); merge != nullptr)
        {
            tile = merge;
        }

        if (t == ret) { ret = tile; }

        return tile;
    },
    [=](Tile* t) { return getTop(t) >= plan.rect.ur.y; },
    [](Tile* t) { return rightTop(t); });

    ret->id = plan.id;

    return ret;
}

CENTO_END_NAMESPACE

#endif // centoInsert_hpp
