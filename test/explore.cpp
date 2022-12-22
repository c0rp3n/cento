//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoExplore.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite empty = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;
        cento::createUniverse(plane);

        const cento::Rect area = {.ll = {.x = -512, .y = -512},
                                  .ur = {.x = 512, .y = 512}};

        // the universe should be empty
        expect(cento::empty(plane, area));
    };

    "single"_test = []()
    {
        cento::Plane plane;

        /*
         * +-----------------------------+ - max
         * |                             |
         * |           above             |
         * |                             |
         * 4---------+---------+---------+ - 256
         * |         |         |         |
         * |  left   | center  |  right  | - 0
         * |         |         |         |
         * 2---------0---------3---------+ - -256
         * |                             |
         * |           below             |
         * |                             |
         * 1-----------------------------+ - min
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
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = min}, .ur = {.x = max, .y = -256}}},
            // tl
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = -256}, .ur = {.x = -256, .y = 256}}},
            // rb
            {.id   = cento::Space,
             .rect = {.ll = {.x = 256, .y = -256}, .ur = {.x = max, .y = 256}}},
            // rt
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = 256}, .ur = {.x = max, .y = max}}},
        };
        const TileVec tiles = createTiles(plane, plan);

        expect(not cento::empty(plane, {.ll = {-128, -128}, .ur = {128, 128}}));
        expect(cento::empty(plane, {.ll = {-500, -128}, .ur = {-300, 128}}));
        expect(cento::empty(plane, {.ll = {300, -128}, .ur = {500, 128}}));

        // corners
        expect(not cento::empty(plane, {.ll = {-400, 0}, .ur = {0, 400}}));
        expect(not cento::empty(plane, {.ll = {0, 0}, .ur = {400, 400}}));
        expect(not cento::empty(plane, {.ll = {-400, -400}, .ur = {0, 0}}));
        expect(not cento::empty(plane, {.ll = {0, -400}, .ur = {400, 0}}));
    };
};

suite neighbours = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;
        cento::Tile* t = cento::createUniverse(plane);

        // the universe should have no left tiles
        i32 count = 0;

        cento::topTiles(t, [&](const cento::Tile*) { ++count; });
        cento::topTiles(t, [&](const cento::Tile*) { ++count; return true; });
        cento::leftTiles(t, [&](const cento::Tile*) { ++count; });
        cento::leftTiles(t, [&](const cento::Tile*) { ++count; return true; });
        cento::bottomTiles(t, [&](const cento::Tile*) { ++count; });
        cento::bottomTiles(t, [&](const cento::Tile*) { ++count; return true; });
        cento::rightTiles(t, [&](const cento::Tile*) { ++count; });
        cento::rightTiles(t, [&](const cento::Tile*) { ++count; return true; });

        expect(count == 0_i);
    };


    "splits"_test = []()
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
        //cento::Tile* const bl     = tiles[1];
        cento::Tile* const tl     = tiles[2];
        cento::Tile* const br     = tiles[3];
        //cento::Tile* const tr     = tiles[4];
        cento::Tile* const lt     = tiles[5];
        //cento::Tile* const rt     = tiles[6];
        //cento::Tile* const lb     = tiles[7];
        cento::Tile* const rb     = tiles[8];

        // Top Tiles

        i32 count = 0;
        cento::topTiles(center, [&](const cento::Tile*) { ++count; });
        expect(count == 2_i);

        count = 0;
        cento::topTiles(center, [&](const cento::Tile* t)
        {
            ++count;
            return t == lt;
        });
        expect(count == 1_i);

        // Left Tiles

        count = 0;
        cento::leftTiles(center, [&](const cento::Tile*) { ++count; });
        expect(count == 2_i);

        count = 0;
        cento::leftTiles(center, [&](const cento::Tile* t)
        {
            ++count;
            return t == tl;
        });
        expect(count == 1_i);

        // Right Tiles

        count = 0;
        cento::rightTiles(center, [&](const cento::Tile*) { ++count; });
        expect(count == 2_i);

        count = 0;
        cento::rightTiles(center, [&](const cento::Tile* t)
        {
            ++count;
            return t == br;
        });
        expect(count == 1_i);

        // Bottom Tiles

        count = 0;
        cento::bottomTiles(center, [&](const cento::Tile*) { ++count; });
        expect(count == 2_i);

        count = 0;
        cento::bottomTiles(center, [&](const cento::Tile* t)
        {
            ++count;
            return t == rb;
        });
        expect(count == 1_i);
    };
};
