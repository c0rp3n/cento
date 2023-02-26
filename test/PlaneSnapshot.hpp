//
// Copyright (C) 2023 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef PlaneSnapshot_hpp
#define PlaneSnapshot_hpp

#pragma once

#include "cento/centoNamespace.hpp"
#include "cento/centoMacros.hpp"
#include "cento/centoExplore.hpp"
#include "cento/centoTilePlan.hpp"
#include "cento/centoPlane.hpp"

#include <map>
#include <set>
#include <optional>

using Name = u64;

inline constexpr Name noName = Name(-1);

struct TileSnapshot
{
    Name        name;
    u64         id;
    cento::Rect rect;

    friend std::strong_ordering operator<=>(const TileSnapshot& lhs, const TileSnapshot& rhs) = default;

    inline friend bool operator<(const TileSnapshot& lhs, const TileSnapshot& rhs) noexcept
    {
        return lhs.rect < rhs.rect;
    }
};

using TileSet = std::set<TileSnapshot>;

inline Name findName(const TileSet& s, const TileSnapshot& ts)
{
    auto it = s.find(ts);
    if (it == s.cend()) { return noName; }

    return it->name;
}

inline Name findName(const TileSet& s, const std::optional<TileSnapshot>& ts)
{
    if (not ts) { return noName; }

    return findName(s, ts.value());
}

inline std::optional<TileSnapshot> getUnnamed(const cento::Tile* t)
{
    if (t == nullptr) { return std::nullopt; }

    return TileSnapshot{noName, t->id, t->rect};
}

using Below = Name;
using Left  = Name;
using Above = Name;
using Right = Name;

struct StitchSnapshot
{
    Name below;
    Name left;
    Name above;
    Name right;

    friend std::strong_ordering operator<=>(const StitchSnapshot& lhs, const StitchSnapshot& rhs) = default;
};

using TileStitchMap = std::map<Name, StitchSnapshot>;

struct PlaneSnapshot
{
    TileSet       tiles;
    TileStitchMap stitches;
};

inline PlaneSnapshot snapshot(const cento::Plane& plane)
{
    PlaneSnapshot s;

    Name name = 0;
    cento::queryAll(plane, [&](cento::Tile* t)
    {
        s.tiles.emplace(name, t->id, t->rect);
        ++name;
    });

    cento::queryAll(plane, [&](cento::Tile* t)
    {
        const Name n = findName(s.tiles, getUnnamed(t));
        if (n == noName) { return; }

        const Name lb = findName(s.tiles, getUnnamed(leftBottom(t)));
        const Name bl = findName(s.tiles, getUnnamed(bottomLeft(t)));
        const Name rt = findName(s.tiles, getUnnamed(rightTop(t)));
        const Name tr = findName(s.tiles, getUnnamed(topRight(t)));

        s.stitches.emplace(n, StitchSnapshot{lb, bl, rt, tr});
    });

    return s;
}

#endif // PlaneSnapshot_hpp
