//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoCreate.hpp"
#include "cento/centoSplit.hpp"

using namespace boost::ut;

suite split_horz = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;

        cento::Tile* const t = cento::createUniverse(plane);

        const cento::HorzSplit split = cento::splitTileHorz(plane, t, 0);

        // check return value
        expect(bool(split));
        expect(split.upper != nullptr);
        expect(split.lower != nullptr);

        using cento::Rect;
        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        // check rects
        expect(split.upper->rect == Rect{.ll = {.x = min, .y = 0},
                                        .ur = {.x = max, .y = max}});
        expect(split.lower->rect == Rect{.ll = {.x = min, .y = min},
                                         .ur = {.x = max, .y = 0}});

        // check they link together
        expect(split.upper->below == split.lower);
        expect(split.lower->above == split.upper);

        // check the rest of the links are nullptr
        expect(split.upper->left  == nullptr);
        expect(split.upper->right == nullptr);
        expect(split.upper->above == nullptr);

        expect(split.lower->left  == nullptr);
        expect(split.lower->right == nullptr);
        expect(split.lower->below == nullptr);
    };
};

suite split_vert = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;

        cento::Tile* const t = cento::createUniverse(plane);

        const cento::VertSplit split = cento::splitTileVert(plane, t, 0);

        // check return value
        expect(bool(split));
        expect(split.left  != nullptr);
        expect(split.right != nullptr);

        using cento::Rect;
        constexpr const i32 min = cento::nInfinity;
        constexpr const i32 max = cento::pInfinity;

        // check rects
        expect(split.left->rect == Rect{.ll = {.x = min, .y = min},
                                        .ur = {.x = 0, .y = max}});
        expect(split.right->rect == Rect{.ll = {.x = 0, .y = min},
                                         .ur = {.x = max, .y = max}});

        // check they link together
        expect(split.left->right == split.right);
        expect(split.right->left == split.left);

        // check the rest of the links are nullptr
        expect(split.left->left  == nullptr);
        expect(split.left->above == nullptr);
        expect(split.left->below == nullptr);

        expect(split.right->right == nullptr);
        expect(split.right->above == nullptr);
        expect(split.right->below == nullptr);
    };
};
