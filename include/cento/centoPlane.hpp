//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoPlane_hpp
#define centoPlane_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoTile.hpp"
#include "centoTilePlan.hpp"

#include <mnta/mnta.hpp>
#include <limits>

CENTO_BEGIN_NAMESPACE

constexpr const i32 pInfinity = std::numeric_limits<i32>::max();
constexpr const i32 nInfinity = std::numeric_limits<i32>::min();

struct Plane
{
    Tile*                      start = nullptr;
    Tile*                      hint  = nullptr;
    mnta::RecyclingArena<Tile> allocator;
};

CENTO_FORCEINLINE Tile* createTile(Plane& plane, const TilePlan& plan)
{
    Tile* const t = plane.allocator.get();
    *t = Tile{.rect = plan.rect,
              .id = plan.id,
              .below = nullptr,
              .left = nullptr,
              .above = nullptr,
              .right = nullptr};

    return t;
}

CENTO_FORCEINLINE Tile* createUniverse(Plane& plane)
{
    const Rect r{.ll = {.x = nInfinity, .y = nInfinity},
                 .ur = {.x = pInfinity, .y = pInfinity}};

    plane.start = createTile(plane, {.id = Space, .rect = r});

    return plane.start;
}

CENTO_END_NAMESPACE

#endif // centoPlane_hpp
