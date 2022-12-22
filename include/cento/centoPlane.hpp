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
    mutable Tile*              hint  = nullptr;
    mnta::RecyclingArena<Tile> allocator;

    CENTO_FORCEINLINE friend Tile* get(Plane& plane) noexcept
    {
        return plane.allocator.get();
    }

    CENTO_FORCEINLINE friend void put(Plane& plane, Tile* tile) noexcept
    {
        plane.allocator.put(tile);
    }
};

CENTO_END_NAMESPACE

#endif // centoPlane_hpp
