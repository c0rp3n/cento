//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoFind_hpp
#define centoFind_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoPlane.hpp"

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE void rightTiles(const Tile* tile, auto callback)
{
    const i32 y = getBottom(tile);

    // 1. Follow the tr stitch of the starting tile to find its topmost right 
    //    neighbour.
    const Tile* next = topRight(tile);

    // 2. Then trace down through lb stitches until the bottommost neighbour is
    //    found (the bottommost neighbour is the one pointed to by the br stitch
    //    of the starting tile).
    while (callback(next))
    {
        next = bottomLeft(next);
        if (getTop(next) < y) { break; }
    }
}

CENTO_FORCEINLINE void leftTiles(const Tile* tile, auto callback)
{
    const i32 y = getTop(tile);

    const Tile* next = leftBottom(tile);
    while (callback(next))
    {
        next = topRight(next);
        if (getBottom(next) > y) { break; }
    }
}

CENTO_END_NAMESPACE

#endif // centoFind_hpp
