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
