//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

#include "cento/centoPoint.hpp"

using namespace boost::ut;

suite point = []()
{
    using cento::Point;

    "equality"_test = []()
    {
        expect(Point{.x = 0, .y = 0}      == Point{.x = 0, .y = 0});
        expect(Point{.x = 256, .y = 0}    == Point{.x = 256, .y = 0});
        expect(Point{.x = 256, .y = 256}  == Point{.x = 256, .y = 256});
        expect(Point{.x = 0, .y = 0}      != Point{.x = 0, .y = 512});
        expect(Point{.x = 512, .y = 0}    != Point{.x = 0, .y = 512});
        expect(Point{.x = 512, .y = -512} != Point{.x = 0, .y = 512});
    };

    "less"_test = []()
    {
        expect((Point{.x = 0, .y = 0} < Point{.x = 0, .y = 0}) == false);

        expect(Point{.x = -256, .y = 0} < Point{.x = 0, .y = 0});
        expect(Point{.x = 0, .y = 0} < Point{.x = 512, .y = 0});
        expect(Point{.x = 0, .y = -512} < Point{.x = 0, .y = 0});
        expect(Point{.x = 0, .y = -256} < Point{.x = 0, .y = 512});

        expect(Point{.x = -256, .y = -512} < Point{.x = 0, .y = 0});
        expect(Point{.x = -256, .y = 512} < Point{.x = 0, .y = 256});

        expect((Point{.x = 256, .y = -512} < Point{.x = 0, .y = 0})   == false);
        expect((Point{.x = 256, .y = -256} < Point{.x = 0, .y = 256}) == false);

        expect(Point{.x = 256, .y = -512} < Point{.x = 256, .y = 0});
        expect(Point{.x = 256, .y = -256} < Point{.x = 256, .y = 256});
    };

    "lesseq"_test = []()
    {
        expect(Point{.x = 0, .y = 0} <= Point{.x = 0, .y = 0});

        expect(Point{.x = -256, .y = 0} <= Point{.x = 0, .y = 0});
        expect(Point{.x = 0, .y = 0}    <= Point{.x = 512, .y = 0});
        expect(Point{.x = 0, .y = -512} <= Point{.x = 0, .y = 0});
        expect(Point{.x = 0, .y = -256} <= Point{.x = 0, .y = 512});

        expect(Point{.x = -256, .y = -512} <= Point{.x = 0, .y = 0});
        expect(Point{.x = -256, .y = 512}  <= Point{.x = 0, .y = 256});

        expect((Point{.x = 256, .y = -512} <= Point{.x = 0, .y = 0})   == false);
        expect((Point{.x = 256, .y = -256} <= Point{.x = 0, .y = 256}) == false);

        expect(Point{.x = 256, .y = -512} <= Point{.x = 256, .y = 0});
        expect(Point{.x = 256, .y = -256} <= Point{.x = 256, .y = 256});
    };
};
