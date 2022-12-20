//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef utils_hpp
#define utils_hpp

#pragma once

#include "cento/centoTilePlan.hpp"
#include "cento/centoPlane.hpp"

#include <iostream>
#include <vector>

using TilingPlan = std::vector<cento::TilePlan>;
using TileVec    = std::vector<cento::Tile*>;

inline bool overlaps(const i32 x1, const i32 x2, const i32 y1, const i32 y2)
{
    return (x1 <= y2) && (y1 <= x2);
}

inline bool isAbove(const cento::Rect& a, const cento::Rect& b)
{
    return (a.ur.y == b.ll.y) && cento::contains(b, a.ur);
}

inline bool isBelow(const cento::Rect& a, const cento::Rect& b)
{
    return (a.ll.y == b.ur.y) && cento::contains(b, a.ll);
}

inline bool isLeft(const cento::Rect& a, const cento::Rect& b)
{
    return (a.ll.x == b.ur.x) && cento::contains(b, a.ll);
}

inline bool isRight(const cento::Rect& a, const cento::Rect& b)
{
    return (a.ur.x == b.ll.x) && cento::contains(b, a.ur);
}

inline void buildStitches(cento::Tile* tile, const TileVec& tiles)
{
    const cento::Rect tr = getRect(tile);
    for (cento::Tile* n : tiles)
    {
        if (n == tile) { continue; }

        const cento::Rect nr = getRect(n);
        if (isAbove(tr, nr))
        {
            tile->above = n;
        }
        else if (isBelow(tr, nr))
        {
            tile->below = n;
        }
        else if (isLeft(tr, nr))
        {
            tile->left = n;
        }
        else if (isRight(tr, nr))
        {
            tile->right = n;
        }
    }
}

inline TileVec createTiles(cento::Plane& plane, const TilingPlan& plan)
{
    TileVec tiles;
    for (const cento::TilePlan& p : plan)
    {
        tiles.push_back(cento::createTile(plane, p));
    }

    for (cento::Tile* tile : tiles)
    {
        buildStitches(tile, tiles);
    }

    return tiles;
}

CENTO_BEGIN_NAMESPACE

std::ostream& operator<<(std::ostream& to, const Point& put)
{
    return to << '{' << put.x << ',' << put.y << '}';
}

std::ostream& operator<<(std::ostream& to, const Rect& put)
{
    return to << '{' << put.ll << ',' << put.ur << '}';
}

std::ostream& operator<<(std::ostream& to, const Tile* put)
{
    if (put == nullptr) { return to << "nullptr"; }

    to << '{' << put->id << ',' << put->rect << ',';
    to << '[' << static_cast<void*>(put->below);
    to << ',' << static_cast<void*>(put->left);
    to << ',' << static_cast<void*>(put->above);
    to << ',' << static_cast<void*>(put->right) << "]}";

    return to;
}

CENTO_END_NAMESPACE

#endif // utils_hpp
