//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoExplore_hpp
#define centoExplore_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoFind.hpp"
#include "centoPlane.hpp"

#include <concepts>
#include <functional>
#include <type_traits>

CENTO_BEGIN_NAMESPACE

namespace detail
{

    template <typename F> requires std::invocable<F&&, Tile*>
    CENTO_FORCEINLINE void iterateTiles(const Tile* tile,
                                        F&&         userCallback,
                                        auto&&      stopCallback,
                                        auto&&      nextTile)
    {
        while (tile)
        {
            if constexpr (std::predicate<F&&, Tile*>)
            {
                if (not std::invoke(std::forward<F>(userCallback), tile)) { return; }
            }
            else
            {
                std::invoke(std::forward<F>(userCallback), tile);
            }

            if (stopCallback(tile)) { return; }

            tile = nextTile(tile);
        }
    }

}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void topTiles(const Tile* tile, F&& callback)
{
    const i32 x = getLeft(tile);

    Tile* left = rightTop(tile);
    detail::iterateTiles(left,
                         std::forward<F>(callback),
                         [&](const Tile* t) { return getLeft(t) <= x; },
                         [](const Tile* t) { return bottomLeft(t); });
}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void leftTiles(const Tile* tile, F&& callback)
{
    const i32 y = getTop(tile);

    Tile* left = bottomLeft(tile);
    detail::iterateTiles(left,
                         std::forward<F>(callback),
                         [&](const Tile* t) { return getTop(t) >= y; },
                         [](const Tile* t) { return topRight(t); });
}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void bottomTiles(const Tile* tile, F&& callback)
{
    const i32 x = getRight(tile);

    Tile* left = leftBottom(tile);
    detail::iterateTiles(left,
                         std::forward<F>(callback),
                         [&](const Tile* t) { return getRight(t) >= x; },
                         [](const Tile* t) { return topRight(t); });
}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void rightTiles(const Tile* tile, F&& callback)
{
    const i32 y = getBottom(tile);

    Tile* right = topRight(tile);
    detail::iterateTiles(right,
                         std::forward<F>(callback),
                         [&](const Tile* t) { return getBottom(t) <= y; },
                         [](const Tile* t) { return leftBottom(t); });
}

CENTO_FORCEINLINE bool empty(const Plane& plane, const Rect& r)
{
    // 1. Use the point finding algorithm to locate the tile containing the
    //    lower-left corner of the area of interest.
    const Tile* tile = findTileAt(plane, r.ll);
    i32         y    = r.ll.y;

    do {
        // 2. See if the tile is solid. If not, it must be a space tile. See if
        //    its right edge is within the area of interest. If so, either it is
        //    the edge of the layout or the edge of a solid tile.
        if (isSolid(tile)) { return false; }

        // 3. If a solid tile was found in step 2, then the search is complete.
        //    If no solid tile was found, then move upwards to the next tile
        //    touching the left edge of the area of interest. This can be done
        //    either by invoking the point-finding algorithm, or by traversing
        //    the lt stitch upwards and then traversing the br stitches right
        //    until the desired tile is found.
        if (getRight(tile) < r.ur.x)
        {
            const Tile* const right = topRight(tile);
            if (isSolid(right) || (getRight(right) < r.ur.x)) { return false; }
        }

        y    = getTop(tile);
        tile = leftTop(tile);

        // 4. Repeat steps 2 and 3 until either a solid tile is found or the top
        //    of the area of interest is reached.
    } while (y < r.ur.y);

    return true;
}

CENTO_END_NAMESPACE

#endif // centoExplore_hpp
