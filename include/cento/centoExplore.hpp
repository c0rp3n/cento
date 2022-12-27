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
    CENTO_FORCEINLINE void iterateTiles(Tile*       tile,
                                        F&&         userCallback,
                                        auto&&      stopCallback,
                                        auto&&      nextTile)
    {
        while (tile)
        {
            if constexpr (std::is_invocable_r_v<Tile*, F&&, Tile*>)
            {
                tile = std::invoke(std::forward<F>(userCallback), tile);
                if (tile == nullptr) { return; }
            }
            else if constexpr (std::predicate<F&&, Tile*>)
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
                         [](const Tile* t) { return rightTop(t); });
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

CENTO_FORCEINLINE bool empty(const Plane& plane, const Rect& area)
{
    // 1. Use the point finding algorithm to locate the tile containing the
    //    lower-left corner of the area of interest.
    cento::Point here = {area.ll.x, area.ur.y - 1};
    cento::Tile* tile = cento::findTileAt(plane, here);

    i64 here_y = here.y;
    while(here_y >= area.ll.y)
    {
        // 2. See if the tile is solid. If not, it must be a space tile. See if
        //    its right edge is within the area of interest. If so, either it is
        //    the edge of the layout or the edge of a solid tile.
        if (isSolid(tile) && (getRight(tile) > area.ll.x)) { return false; }

        // 3. If a solid tile was found in step 2, then the search is complete.
        //    If no solid tile was found, then move upwards to the next tile
        //    touching the left edge of the area of interest. This can be done
        //    either by invoking the point-finding algorithm, or by traversing
        //    the lt stitch upwards and then traversing the br stitches right
        //    until the desired tile is found.
        if (getRight(tile) < area.ur.x)
        {
            const Tile* const right = topRight(tile);
            if (isSolid(right) || (getRight(right) < area.ur.x)) { return false; }
        }

        here_y = i64(getBottom(tile)) - 1;
        here.y = i32(here_y);
        tile   = cento::findTileAt(plane, here);

        // 4. Repeat steps 2 and 3 until either a solid tile is found or the top
        //    of the area of interest is reached.
    }

    return true;
}

namespace detail
{

    template <typename F> requires std::invocable<F&&, Tile*>
    bool areaEnum(Tile*       enumRT,
                  i32         enumBottom,
                  const Rect& area,
                  F&&         callback)
    {
        const bool atBottom = (enumBottom <= area.ll.y);

        /*
         * Begin examination of tiles along right edge.
         * A tile to the right of the one being enumerated is enumerable if:
         *  - its bottom lies at or above that of the tile being enumerated, or,
         *  - the bottom of the tile being enumerated lies at or below the
         *    bottom of the search rectangle.
         */

        i32 srchBottom = enumBottom;
        if (srchBottom < area.ll.y) { srchBottom = area.ll.y; }

        Tile* tp;
        Tile* tpLB;
        i32   tpNextTop;
        for (tp = enumRT, tpNextTop = getTop(tp); tpNextTop > srchBottom; tp = tpLB)
        {
            /*
             * Since the client's filter function may result in this tile
             * being deallocated or otherwise modified, we must extract
             * all the information we will need from the tile before we
             * apply the filter function.
             */

            tpLB      = leftBottom(tp);
            tpNextTop = tpLB ? getTop(tpLB) : nInfinity; /* Since getTop(tpLB) comes from tp */

            if ((getBottom(tp) < area.ur.y) && (atBottom || (getBottom(tp) >= enumBottom)))
            {
                /*
                 * We extract more information from the tile, which we will use
                 * after applying the filter function.
                 */

                i32   tpRight  = getRight(tp);
                i32   tpBottom = getBottom(tp);
                Tile* tpTR     = topRight(tp);

                if constexpr (std::predicate<F&&, Tile*>)
                {
                    if (not std::invoke(std::forward<F>(callback), tp)) { return true; }
                }
                else
                {
                    std::invoke(std::forward<F>(callback), tp);
                }

                /*
                 * If the right boundary of the tile being enumerated is
                 * inside of the search area, recursively enumerate
                 * tiles to its right.
                 */

                if (tpRight < area.ur.x)
                {
                    if (areaEnum(tpTR, tpBottom, area, std::forward<F>(callback))) { return true; }
                }
            }
        }

        return false;
    }

}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void query(const Plane& plane, const Rect& area, F&& callback)
{
    cento::Point here     = {area.ll.x, area.ur.y - 1};
    cento::Tile* enumTile = cento::findTileAt(plane, here);

    i64 here_y = here.y;
    while(here_y >= area.ll.y)
    {
        /*
         * Find the tile (tp) immediately below the one to be
         * enumerated (enumTile).  This must be done before we enumerate
         * the tile, as the filter function applied to enumerate
         * it can result in its deallocation or modification in
         * some other way.
         * 
         * We also have to be sure we do not overflow from the infinity tile
         */

        here_y          = i64(getBottom(enumTile)) - 1;
        here.y          = i32(here_y);
        cento::Tile* tp = cento::findTileAt(plane, here);

        cento::Point enumRB = {getRight(enumTile), getBottom(enumTile)};
        cento::Tile* enumTR = topRight(enumTile);

        if constexpr (std::predicate<F&&, Tile*>)
        {
            if (not std::invoke(std::forward<F>(callback), enumTile)) { return; }
        }
        else
        {
            std::invoke(std::forward<F>(callback), enumTile);
        }

        /*
         * If the right boundary of the tile being enumerated is
         * inside of the search area, recursively enumerate
         * tiles to its right.
         */

        if (enumRB.x < area.ur.x)
        {
            if (detail::areaEnum(enumTR, enumRB.y, area, std::forward<F>(callback))) { return; }
        }
        enumTile = tp;
    }
}

template <typename F> requires std::invocable<F&&, Tile*>
CENTO_FORCEINLINE void queryAll(const Plane& plane, F&& callback)
{
    const i32  min = nInfinity + 1;
    const i32  max = pInfinity - 1;
    const Rect area{{min, min}, {max, max}};

    query(plane, area, std::forward<F>(callback));
}

CENTO_END_NAMESPACE

#endif // centoExplore_hpp
