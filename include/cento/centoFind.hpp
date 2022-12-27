//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoFind_hpp
#define centoFind_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoPlane.hpp"

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE Tile* findTileAt(Tile*        start,
                                   const Point& point)
{
    Tile* t = start;

    // 1. First move up (or down) along the left edges of tiles until a tile
    //    is found whose vertical range contains the desired point.
    if (point.y < getBottom(t))
    {
        do { t = leftBottom(t); } while (t && (point.y < getBottom(t)));
    }
    else
    {
        while (t && (point.y >= getTop(t))) { t = rightTop(t); }
    }

    if (t == nullptr) { return nullptr; }

    // 2. Then move left (or right) along the bottom edges of tiles until a
    //    tile is found whose horizontal range contains the desired point.
    if (point.x < getLeft(t))
    {
        do
        {
            do { t = bottomLeft(t); } while (t && (point.x < getLeft(t)));
            if ((t == nullptr) || (point.y < getTop(t))) { break; }
            do { t = rightTop(t); } while (point.y >= getTop(t));
        } while (point.x < getLeft(t));

        // 3. Since the horizontal motion may have caused a vertical
        //    misalignment, steps 1 and 2 may have to be iterated several times
        //    to locate the tile containing the point.
    }
    else
    {
        while (point.x >= getRight(t))
        {
            do t = topRight(t); while (t && (point.x >= getRight(t)));
            if ((t == nullptr) || (point.y >= getBottom(t))) { break; }
            do t = leftBottom(t); while (t && (point.y < getBottom(t)));
            if (t == nullptr) { break; }
        }
    }

    return t;
}

CENTO_FORCEINLINE Tile* findTileAt(const Plane& plane, const Point& point)
{
    Tile* t = findTileAt(plane.hint, point);
    if (t) { plane.hint = t; }

    return t;
}

CENTO_END_NAMESPACE

#endif // centoFind_hpp
