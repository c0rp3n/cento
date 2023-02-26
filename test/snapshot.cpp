//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/cento.hpp"
#include "cento/centoInsert.hpp"
#include "cento/centoRemove.hpp"

#include "PlaneSnapshot.hpp"
#include "utils.hpp"

using namespace boost::ut;

suite snapshot_t = []()
{
    "universe"_test = []()
    {
        cento::Plane plane;
        cento::createUniverse(plane);

        PlaneSnapshot s = snapshot(plane);
        expect(s.tiles.size()    == 1_i);
        expect(s.stitches.size() == 1_i);

        const PlaneSnapshot expected{
            TileSet{
                {Name{0}, cento::Space, cento::universeRect},
            },
            TileStitchMap{
                {Name{0}, {Below{noName}, Left{noName}, Above{noName}, Left{noName}}},
            },
        };
        // @TODO add operator==() overload for PlaneSnapshot this should look to
        //       use a set difference on the two tile sets and then build a map
        //       of tile names between the two snapshots
    };
};
