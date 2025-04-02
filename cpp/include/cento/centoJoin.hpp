//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoJoin_hpp
#define centoJoin_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoPlane.hpp"

#include <gsl/assert>

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE Tile* joinTileHorz(Plane& plane, Tile* upper, Tile* lower)
{
    /*
     * Basic algorithm:
     *
     *  Update all the corner stitches in the neighbors of right
     *  to point to left.
     *  Update the corner stitches of left along the shared edge
     *  to be those of right.
     *  Change the bottom or left coordinate of left if appropriate.
     *  Deallocate right.
     */

    Expects(getLeft(upper)   == getLeft(lower));
    Expects(getRight(upper)  == getRight(lower));
    Expects(getBottom(upper) == getTop(lower));

    Tile* tp = nullptr;

    /*
     * Update stitches along right of tile.
     */

    for (tp = topRight(upper); tp && bottomLeft(tp) == upper; tp = leftBottom(tp))
    {
        bottomLeft(tp) = lower;
    }

    /*
     * Update stitches along left of tile.
     */

    for (tp = bottomLeft(upper); tp && topRight(tp) == upper; tp = rightTop(tp))
    {
        topRight(tp) = lower;
    }

    for (tp = rightTop(upper); tp && leftBottom(tp) == upper; tp = bottomLeft(tp))
    {
        leftBottom(tp) = lower;
    }
    rightTop(lower) = rightTop(upper);
    topRight(lower) = topRight(upper);
    setTop(lower, getTop(upper));

    if (plane.hint == upper) { plane.hint = lower; }
    put(plane, upper);

    return lower;
}

CENTO_FORCEINLINE Tile* joinTileVert(Plane& plane, Tile* left, Tile* right)
{
    /*
     * Basic algorithm:
     *
     *  Update all the corner stitches in the neighbors of right
     *  to point to left.
     *  Update the corner stitches of left along the shared edge
     *  to be those of right.
     *  Change the bottom or left coordinate of left if appropriate.
     *  Deallocate right.
     */

    Expects(getTop(left)    == getTop(right));
    Expects(getBottom(left) == getBottom(right));
    Expects(getRight(left)  == getLeft(right));

    Tile* tp = nullptr;

    /*
     * Update stitches along top of tile
     */

    for (tp = rightTop(right); tp && leftBottom(tp) == right; tp = bottomLeft(tp))
    {
        leftBottom(tp) = left;
    }

    /*
     * Update stitches along bottom of tile
     */

    for (tp = leftBottom(right); tp && rightTop(tp) == right; tp = topRight(tp))
    {
        rightTop(tp) = left;
    }

    /*
     * Update stitches along either left or right, depending
     * on relative position of the two tiles.
     */

    for (tp = topRight(right); tp && bottomLeft(tp) == right; tp = leftBottom(tp))
    {
        bottomLeft(tp) = left;
    }
    topRight(left) = topRight(right);
    rightTop(left) = rightTop(right);
    setRight(left, getRight(right));

    if (plane.hint == right) { plane.hint = left; }
    put(plane, right);

    return left;
}

CENTO_END_NAMESPACE

#endif // centoJoin_hpp
