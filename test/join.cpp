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

suite join = []()
{
    "side_horz"_test = []()
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

    "side_vert"_test = []()
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

    "merge"_test = []()
    {
        cento::Plane plane;

        /*
         * +--------------+--------------+ - max
         * |              |              |
         * |      lt      |      rt      |
         * |              |              |
         * +---------+----+----+---------+ - 256
         * |    tl   |         |   tr    |
         * +---------+ center  +---------+ - 0
         * |    bl   |         |   br    |
         * +---------+----+----+---------+ - -256
         * |              |              |
         * |      lb      |      rb      |
         * |              |              |
         * +--------------+--------------+ - min
         *
         * m         -    0    2         m
         * i         2         5         a
         * n         5         6         x
         *           6
         *
         */

        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        const TilingPlan plan =
        {
            // center
            {.id   = 0,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 256, .y = 256}}},
            // bl
            {.id   = 1,
             .rect = {.ll = {.x = min, .y = -256}, .ur = {.x = -256, .y = 0}}},
            // tl
            {.id   = 2,
             .rect = {.ll = {.x = min, .y = 0}, .ur = {.x = -256, .y = 256}}},
            // rb
            {.id   = 3,
             .rect = {.ll = {.x = 256, .y = -256}, .ur = {.x = max, .y = 0}}},
            // rt
            {.id   = 4,
             .rect = {.ll = {.x = 256, .y = 0}, .ur = {.x = max, .y = 256}}},
            // tl
            {.id   = 5,
             .rect = {.ll = {.x = min, .y = 256}, .ur = {.x = 0, .y = max}}},
            // tr
            {.id   = 6,
             .rect = {.ll = {.x = 0, .y = 256}, .ur = {.x = max, .y = max}}},
            // bl
            {.id   = 7,
             .rect = {.ll = {.x = min, .y = min}, .ur = {.x = 0, .y = -256}}},
            // br
            {.id   = 8,
             .rect = {.ll = {.x = 0, .y = min}, .ur = {.x = max, .y = -256}}},
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const center = tiles[0];
        cento::Tile* const bl     = tiles[1];
        cento::Tile* const tl     = tiles[2];
        cento::Tile* const br     = tiles[3];
        cento::Tile* const tr     = tiles[4];
        cento::Tile* const lt     = tiles[5];
        cento::Tile* const rt     = tiles[6];
        cento::Tile* const lb     = tiles[7];
        cento::Tile* const rb     = tiles[8];

        /*
         * +-----------------------------+ - max
         * |                             |
         * |            above            |
         * |                             |
         * +---------+---------+---------+ - 256
         * |         |         |         |
         * |  left   | center  |  right  | - 0
         * |         |         |         |
         * +---------+---------+---------+ - -256
         * |                             |
         * |            below            |
         * |                             |
         * +-----------------------------+ - min
         *
         * m         -    0    2         m
         * i         2         5         a
         * n         5         6         x
         *           6
         *
         */

        cento::Tile* const left  = cento::joinTileHorz(plane, tl, bl);
        cento::Tile* const right = cento::joinTileHorz(plane, tr, br);
        cento::Tile* const above = cento::joinTileVert(plane, lt, rt);
        cento::Tile* const below = cento::joinTileVert(plane, lb, rb);

        expect(left  != nullptr);
        expect(right != nullptr);
        expect(above != nullptr);
        expect(below != nullptr);

        using Stitches = cento::Stitches;
        expect(getStitches(center) == Stitches{below, left, above, right});
        expect(getStitches(left)   == Stitches{below, nullptr, above, center});
        expect(getStitches(right)  == Stitches{below, center, above, nullptr});
        expect(getStitches(above)  == Stitches{left, nullptr, nullptr, nullptr});
        expect(getStitches(below)  == Stitches{nullptr, nullptr, right, nullptr});
    };
};
