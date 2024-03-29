//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoZPlane_hpp
#define centoZPlane_hpp

#pragma once

#include "centoNamespace.hpp"
#include "centoMacros.hpp"
#include "centoZTile.hpp"

#include <vector>

CENTO_BEGIN_NAMESPACE

struct ZPlane
{
    std::vector<ZTile>  tiles;
    std::vector<ZTile&> freeTiles;
};

CENTO_END_NAMESPACE

#endif // centoZPlane_hpp
