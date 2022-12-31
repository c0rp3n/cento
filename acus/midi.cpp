//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#include "midi.hpp"

#include <fmt/format.h>

#include <lexy/action/parse.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/file.hpp>

#include <lexy_ext/report_error.hpp>

namespace
{

namespace ast
{

    struct poly
    {
        i32 coords[8];

        constexpr poly(i32 c0, i32 c1, i32 c2, i32 c3, i32 c4, i32 c5, i32 c6, i32 c7)
            : coords(c0, c1, c2, c3, c4, c5, c6, c7)
        {
        }
    };

}

namespace grammar
{

    namespace dsl = lexy::dsl;

    // An integer literal.
    // Supports decimal literals.
    struct integer : lexy::token_production
    {
        static constexpr auto rule  = dsl::integer<i32>;
        static constexpr auto value = lexy::forward<i32>;
    };

    // Comment.
    struct comment
    {
        static constexpr auto rule  = LEXY_LIT("#") >> dsl::until(dsl::ascii::newline);
        static constexpr auto value = lexy::forward<void>;
    };

    constexpr auto ws = dsl::whitespace(dsl::ascii::space);

    struct line
    {
        static constexpr auto rule  = dsl::times<8>(ws + dsl::p<integer>) + dsl::eol;
        static constexpr auto value = lexy::construct<ast::poly>;
    };

    struct document
    {
        static constexpr auto rule = []()
        {
            return dsl::brackets(dsl::p<comment>, dsl::p<comment>).opt_list(dsl::p<line>);
        }();

        static constexpr auto value = lexy::as_list<std::vector<ast::poly>>;
    };

}

}

std::vector<cento::Rect> parseMidi(const std::string_view path)
{
    auto file = lexy::read_file<lexy::utf8_encoding>(path.data());
    if (!file) { return {}; }

    const auto document = lexy::parse<grammar::document>(
        file.buffer(), lexy_ext::report_error.path(path.data()));
    if (!document) { return {}; }

    std::vector<cento::Rect> rects;

    const std::vector<ast::poly>& polygons = document.value();
    for (const ast::poly& poly : polygons)
    {
        const i32 (&coor)[8] = poly.coords;
        if ((coor[0] != coor[2]) ||
            (coor[3] != coor[5]) ||
            (coor[4] != coor[6]) ||
            (coor[7] != coor[1]))
        {
            fmt::print(stderr,
                       "Skipped invalid rectangle: ({}, {}) - ({}, {}) - ({}, {}) - ({}, {})\n",
                       coor[0],
                       coor[1],
                       coor[2],
                       coor[3],
                       coor[4],
                       coor[5],
                       coor[6],
                       coor[7]);
            continue;
        }

        // collect tile
        rects.push_back({.ll = {coor[0], coor[1]}, .ur = {coor[4], coor[5]}});
    }

    return rects;
}
