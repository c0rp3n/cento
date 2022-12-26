//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoRemove.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite removeT = []()
{
    "single"_test = []()
    {
        cento::Plane plane;

        /*
         * +-----------------------------+ 
         * |                             |
         * |            above            |
         * |                             |
         * +---------+---------+---------+
         * |         |         |         |
         * |  left   | center  |  right  |
         * |         |         |         |
         * +---------+---------+---------+
         * |                             |
         * |            below            |
         * |                             |
         * +-----------------------------+
         *
         */

        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        const TilingPlan plan =
        {
            // center
            {.id   = 0,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 256, .y = 256}}},
            // left
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = -256}, .ur = {.x = -256, .y = 256}}},
            // right
            {.id   = cento::Space,
             .rect = {.ll = {.x = 256, .y = -256}, .ur = {.x = max, .y = 256}}},
            // top
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = 256}, .ur = {.x = max, .y = max}}},
            // bottom
            {.id   = cento::Space,
             .rect = {.ll = {.x = min, .y = min}, .ur = {.x = max, .y = -256}}},
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const center = tiles[0];
        plane.hint = center;

        cento::removeTile(plane, center);

        expect(plane.hint->id   == cento::Space);
        expect(plane.hint->rect == cento::Rect{{min, min}, {max, max}});
    };

    "surrounded"_test = []()
    {
        cento::Plane plane;

        /*
         * +-----------------------------+ 
         * |                             |
         * |            above            |
         * |                             |
         * +---------+---------+---------+
         * |         |         |         |
         * |  left   | center  |  right  |
         * |         |         |         |
         * +---------+---------+---------+
         * |                             |
         * |            below            |
         * |                             |
         * +-----------------------------+
         *
         */

        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        const TilingPlan plan =
        {
            // center
            {.id   = 0,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 256, .y = 256}}},
            // left
            {.id   = 1,
             .rect = {.ll = {.x = min, .y = -256}, .ur = {.x = -256, .y = 256}}},
            // right
            {.id   = 2,
             .rect = {.ll = {.x = 256, .y = -256}, .ur = {.x = max, .y = 256}}},
            // top
            {.id   = 3,
             .rect = {.ll = {.x = min, .y = 256}, .ur = {.x = max, .y = max}}},
            // bottom
            {.id   = 4,
             .rect = {.ll = {.x = min, .y = min}, .ur = {.x = max, .y = -256}}},
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const center = tiles[0];
        plane.hint = center;

        cento::removeTile(plane, center);

        expect(plane.hint->id   == cento::Space);
        expect(plane.hint->rect == cento::Rect{{-256, -256}, {256, 256}});
    };
};
