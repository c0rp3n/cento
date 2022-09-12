//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/centoRect.hpp"
#include "cento/centoPlane.hpp"

using namespace boost::ut;

suite rect = []()
{
    using cento::Point;
    using cento::Rect;

    "equality"_test = []()
    {
        Point zero{.x = 0, .y = 0};
        Point midLeft{.x = 256, .y = 0};
        Point upperRight{.x = 256, .y = 256};
        Point lowerLeft{.x = -512, .y = -512};

        expect(Rect{.ll = zero, .ur = zero}       == Rect{.ll = zero, .ur = zero});
        expect(Rect{.ll = zero, .ur = midLeft}    == Rect{.ll = zero, .ur = midLeft});
        expect(Rect{.ll = zero, .ur = upperRight} == Rect{.ll = zero, .ur = upperRight});

        expect(Rect{.ll = zero, .ur = zero}         != Rect{.ll = zero, .ur = midLeft});
        expect(Rect{.ll = zero, .ur = midLeft}      != Rect{.ll = zero, .ur = zero});
        expect(Rect{.ll = lowerLeft, .ur = midLeft} != Rect{.ll = zero, .ur = upperRight});
    };

    "contains"_test = []()
    {
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

        const Rect tl{.ll = {.x = min, .y = 0},
                      .ur = {.x = 0, .y = max}};

        const Rect tr{.ll = {.x = 0, .y = 0},
                      .ur = {.x = max, .y = max}};

        const Rect bl{.ll = {.x = min, .y = min},
                      .ur = {.x = 0, .y = 0}};

        const Rect br{.ll = {.x = 0, .y = min},
                      .ur = {.x = max, .y = 0}};

        const Point cen = {0,0};
        expect(contains(tl, cen));
        expect(contains(tr, cen));
        expect(contains(bl, cen));
        expect(contains(br, cen));

        const Point tlp = {-256,256};
        expect(    contains(tl, tlp));
        expect(not contains(tr, tlp));
        expect(not contains(bl, tlp));
        expect(not contains(br, tlp));

        const Point trp = {256,256};
        expect(not contains(tl, trp));
        expect(    contains(tr, trp));
        expect(not contains(bl, trp));
        expect(not contains(br, trp));

        const Point blp = {-256,-256};
        expect(not contains(tl, blp));
        expect(not contains(tr, blp));
        expect(    contains(bl, blp));
        expect(not contains(br, blp));

        const Point brp = {256,-256};
        expect(not contains(tl, brp));
        expect(not contains(tr, brp));
        expect(not contains(bl, brp));
        expect(    contains(br, brp));
    };
};
