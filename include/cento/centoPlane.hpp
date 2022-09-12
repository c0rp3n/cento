//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoPlane_hpp
#define centoPlane_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoTile.hpp"

#include <mnta/mnta.hpp>

CENTO_BEGIN_NAMESPACE

struct Plane
{
    Tile*                      start;
    mnta::RecyclingArena<Tile> allocator;
};

CENTO_END_NAMESPACE

#endif // centoPlane_hpp
