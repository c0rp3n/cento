//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoCreate.hpp"
#include "cento/centoFind.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite create_tiles = []()
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

        expect(eq(topRight(tiles[0]), tiles[1]));
        expect(eq(bottomLeft(tiles[1]), tiles[0]));
    };
};

suite tile_stitches = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;

        cento::Tile* const       t  = cento::createUniverse(plane);
        const cento::Tile* const tc = t;

        expect(bottomLeft(t) == nullptr);
        expect(leftBottom(t) == nullptr);
        expect(topRight(t) == nullptr);
        expect(rightTop(t) == nullptr);

        expect(bottomLeft(tc) == nullptr);
        expect(leftBottom(tc) == nullptr);
        expect(topRight(tc) == nullptr);
        expect(rightTop(tc) == nullptr);
    };

    "unique"_test = []()
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

        const cento::Tile* const center = tiles[0];
        const cento::Tile* const bl     = tiles[1];
        const cento::Tile* const tl     = tiles[2];
        const cento::Tile* const br     = tiles[3];
        const cento::Tile* const tr     = tiles[4];
        const cento::Tile* const lt     = tiles[5];
        const cento::Tile* const rt     = tiles[6];
        const cento::Tile* const lb     = tiles[7];
        const cento::Tile* const rb     = tiles[8];

        expect(eq(bottomLeft(center), bl));
        expect(eq(topLeft(center), tl));
        expect(eq(bottomRight(center), br));
        expect(eq(topRight(center), tr));
        expect(eq(leftTop(center), lt));
        expect(eq(rightTop(center), rt));
        expect(eq(leftBottom(center), lb));
        expect(eq(rightBottom(center), rb));
    };
};
