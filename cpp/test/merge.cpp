//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoMerge.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite merge = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;

        cento::Tile* const t = cento::createUniverse(plane);

        // the universe should not be mergeable
        expect(cento::mergeUp(plane, t)    == nullptr);
        expect(cento::mergeRight(plane, t) == nullptr);
        expect(cento::mergeDown(plane, t)  == nullptr);
        expect(cento::mergeLeft(plane, t)  == nullptr);
    };

    "ids_not_match"_test = []()
    {
        cento::Plane plane;

        /*
         * +-------+
         * | above |
         * +-------+
         * | below |
         * +-------+
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

        expect(cento::mergeUp(plane, below)   == nullptr);
        expect(cento::mergeDown(plane, above) == nullptr);
    };

    "ids_match"_test = []()
    {
        cento::Plane plane;

        /*
         * +-------+
         * | above |
         * +-------+
         * | below |
         * +-------+
         *
         */
        const TilingPlan plan =
        {
            {.id   = cento::Space,
             .rect = {.ll = {.x = -256, .y = -256}, .ur = {.x = 256, .y = 0}}},
            {.id   = cento::Space,
             .rect = {.ll = {.x = -256, .y = 0}, .ur = {.x = 256, .y = 256}}}
        };
        const TileVec tiles = createTiles(plane, plan);

        cento::Tile* const below = tiles[0];

        expect(cento::mergeUp(plane, below));
    };
};
