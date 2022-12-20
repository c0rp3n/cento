//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoFind.hpp"

using namespace boost::ut;

suite find = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;

        const cento::Tile* const t = cento::createUniverse(plane);

        expect(cento::findTileAt(plane, {0,0})    == t);
        expect(cento::findTileAt(plane, {-512,0}) == t);
        expect(cento::findTileAt(plane, {512,0})  == t);
        expect(cento::findTileAt(plane, {0,-512}) == t);
        expect(cento::findTileAt(plane, {0,512})  == t);
    };

    "axis"_test = []()
    {
        cento::Plane plane;

        /*
         *         :
         *         :
         *    tl   :   tr
         *         :
         *         :
         * - - - - + - - - -
         *         :
         *         :
         *    bl   :   br
         *         :
         *         :
         *
         */

        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        const cento::TilePlan tlp{.id   = 0,
                                  .rect = {.ll = {.x = min, .y = 0},
                                           .ur = {.x = 0, .y = max}}};

        const cento::TilePlan trp{.id   = 1,
                                  .rect = {.ll = {.x = 0, .y = 0},
                                           .ur = {.x = max, .y = max}}};

        const cento::TilePlan blp{.id   = 2,
                                  .rect = {.ll = {.x = min, .y = min},
                                           .ur = {.x = 0, .y = 0}}};

        const cento::TilePlan brp{.id   = 3,
                                  .rect = {.ll = {.x = 0, .y = min},
                                           .ur = {.x = max, .y = 0}}};

        cento::Tile* const tl = cento::createTile(plane, tlp);
        cento::Tile* const tr = cento::createTile(plane, trp);
        cento::Tile* const bl = cento::createTile(plane, blp);
        cento::Tile* const br = cento::createTile(plane, brp);

        tl->below = bl;
        tl->right = tr;

        tr->below = br;
        tr->left  = tl;

        bl->above = tl;
        bl->right = br;

        br->above = tr;
        br->left  = bl;

        plane.start = bl;

        expect(cento::findTileAt(plane, {0,0})       == bl);
        expect(cento::findTileAt(plane, {-256,256})  == tl);
        expect(cento::findTileAt(plane, {256,256})   == tr);
        expect(cento::findTileAt(plane, {-256,-256}) == bl);
        expect(cento::findTileAt(plane, {256,-256})  == br);
    };
};
