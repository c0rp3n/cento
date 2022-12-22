//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoMerge_hpp
#define centoMerge_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoJoin.hpp"
#include "centoPlane.hpp"

#include <gsl/assert>

CENTO_BEGIN_NAMESPACE

namespace detail
{

    CENTO_FORCEINLINE bool canMergeHorz(const Tile* above, const Tile* below)
    {
        // The left tiles right edge should always lie on the left edge of tile
        Expects(getBottom(above) == getTop(below));

        return (getLeft(above) == getLeft(below)) &&
               (getRight(above) == getRight(below));
    }

    CENTO_FORCEINLINE bool canMergeVert(const Tile* left, const Tile* right)
    {
        // The left tiles right edge should always lie on the left edge of tile
        Expects(getRight(left) == getLeft(right));

        return (getTop(left) == getTop(right)) &&
               (getBottom(left) == getBottom(right));
    }

    CENTO_FORCEINLINE bool canMergeIDs(const Tile* lhs, const Tile* rhs)
    {
        // Check whether the IDs of the tiles can be merged
        // This should be controlled by some customisation point in the future
        return lhs->id == rhs->id;
    }

}

CENTO_FORCEINLINE Tile* mergeUp(Plane& plane, Tile* tile)
{
    Tile* above = rightTop(tile);
    if (above == nullptr) { return nullptr; }

    // Check whether the heigh of the tiles matches
    if (not detail::canMergeHorz(above, tile)) { return nullptr; }

    // Check whether the IDs of the tiles can be merged
    // This should be controlled by some customisation point in the future
    if (not detail::canMergeIDs(above, tile)) { return nullptr; }

    return joinTileHorz(plane, above, tile);
}

CENTO_FORCEINLINE Tile* mergeLeft(Plane& plane, Tile* tile)
{
    Tile* left = bottomLeft(tile);
    if (left == nullptr) { return nullptr; }

    // Check whether the height of the tiles matches
    if (not detail::canMergeVert(left, tile)) { return nullptr; }

    // Check whether the IDs of the tiles can be merged
    // This should be controlled by some customisation point in the future
    if (not detail::canMergeIDs(left, tile)) { return nullptr; }

    return joinTileVert(plane, left, tile);
}

CENTO_FORCEINLINE Tile* mergeDown(Plane& plane, Tile* tile)
{
    Tile* below = leftBottom(tile);
    if (below == nullptr) { return nullptr; }

    // Check whether the width of the tiles matches
    if (not detail::canMergeHorz(tile, below)) { return nullptr; }

    // Check whether the IDs of the tiles can be merged
    // This should be controlled by some customisation point in the future
    if (not detail::canMergeIDs(tile, below)) { return nullptr; }

    return joinTileHorz(plane, tile, below);
}

CENTO_FORCEINLINE Tile* mergeRight(Plane& plane, Tile* tile)
{
    Tile* right = topRight(tile);
    if (right == nullptr) { return nullptr; }

    // Check whether the height of the tiles matches
    if (not detail::canMergeVert(tile, right)) { return nullptr; }

    // Check whether the IDs of the tiles can be merged
    // This should be controlled by some customisation point in the future
    if (not detail::canMergeIDs(tile, right)) { return nullptr; }

    return joinTileVert(plane, tile, right);
}

CENTO_END_NAMESPACE

#endif // centoMerge_hpp
