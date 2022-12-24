//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoInsert.hpp"

#include "utils.hpp"

using namespace boost::ut;

suite insert = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;
        cento::createUniverse(plane);

        const cento::TilePlan    plan{.id = 0, .rect = {{-256, -256}, {256, 256}}};
        const cento::Tile* const tile = cento::insertTile(plane, plan);

        expect(tile->id         == plan.id);
        expect(getRect(tile)    == plan.rect);

        expect(leftBottom(tile) != nullptr);
        expect(bottomLeft(tile) != nullptr);
        expect(rightTop(tile)   != nullptr);
        expect(topRight(tile)   != nullptr);

        expect(leftBottom(tile)->id == cento::Space);
        expect(bottomLeft(tile)->id == cento::Space);
        expect(rightTop(tile)->id   == cento::Space);
        expect(topRight(tile)->id   == cento::Space);
    };
};
