//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoExplore.hpp"

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
};
