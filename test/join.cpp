//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoJoin.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite join_horz = []()
{
    "side"_test = []()
    {
        cento::Plane plane;

        /*
         * +-------+
         * | above |
         * +-------+
         * | below |
         * +-------+
         *
         *
         */
        const TilingPlan plan =
        {
            {.id   = 0,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 256, .y = 0}}},
            {.id   = 1,
             .rect = {.ll = {.x = -256, .y = 0}, .ur = {.x = 256, .y = 256}}}
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const below = tiles[0];
        cento::Tile* const above = tiles[1];

        cento::Tile* t = cento::joinTileHorz(plane, above, below);
        expect(t != nullptr);
        expect(t == below);
        expect(getRect(t) == cento::Rect{.ll = {-256, -256}, .ur = {256, 256}});
    };
};

suite join_vert = []()
{
    "side"_test = []()
    {
        cento::Plane plane;

        /*
         * +-------+-------+
         * |  left | right |
         * +-------+-------+
         *
         *
         */
        const TilingPlan plan =
        {
            {.id   = 0,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 0, .y = 256}}},
            {.id   = 1,
             .rect = {.ll = {.x = 0, .y = -256}, .ur = {.x = 256, .y = 256}}}
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const left  = tiles[0];
        cento::Tile* const right = tiles[1];

        cento::Tile* t = cento::joinTileVert(plane, left, right);
        expect(t != nullptr);
        expect(t == left);
        expect(getRect(t) == cento::Rect{.ll = {-256, -256}, .ur = {256, 256}});
    };
};
