//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoCreate_hpp
#define centoCreate_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoExplore.hpp"
#include "centoPlane.hpp"

CENTO_BEGIN_NAMESPACE

CENTO_FORCEINLINE Tile* createTile(Plane& plane, const TilePlan& plan)
{
    cento::Tile* ret = get(plane);
    *ret = Tile{.rect = plan.rect,
                .id = plan.id,
                .below = nullptr,
                .left = nullptr,
                .above = nullptr,
                .right = nullptr};

    return ret;
}

CENTO_FORCEINLINE Tile* createUniverse(Plane& plane)
{
    const Rect r{.ll = {.x = nInfinity, .y = nInfinity},
                 .ur = {.x = pInfinity, .y = pInfinity}};

    plane.start  = get(plane);
    *plane.start = Tile{.rect = r,
                        .id = Space,
                        .below = nullptr,
                        .left = nullptr,
                        .above = nullptr,
                        .right = nullptr};

    return plane.start;
}

CENTO_END_NAMESPACE

#endif // centoCreate_hpp
