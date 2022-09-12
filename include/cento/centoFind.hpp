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

#include <mnta/mnta.hpp>

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE Tile* findTileAt(const Plane& plane, const Point& point)
{
    Tile* t = plane.start;

    while (not contains(rect(t), point))
    {
        // 1. First move up (or down) along the left edges of tiles until a tile
        //    is found whose vertical range contains the desired point.
        while (upperRight(rect(t)).y < point.y) { t = t->above; }
        while (lowerLeft(rect(t)).y > point.y) { t = t->below; }

        // 2. Then move left (or right) along the bottom edges of tiles until a
        //    tile is found whose horizontal range contains the desired point.
        while (upperRight(rect(t)).x < point.x) { t = t->right; }
        while (lowerLeft(rect(t)).y > point.x) { t = t->left; }

        // 3. Since the horizontal motion may have caused a vertical
        //    misalignment, steps 1 and 2 may have to be iterated several times
        //    to locate the tile containing the point.
    }

    return t;
}

CENTO_END_NAMESPACE

#endif // centoFind_hpp
