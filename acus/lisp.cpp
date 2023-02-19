//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#include "lisp.hpp"

#include <variant>
#include <fmt/format.h>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/file.hpp>
#include <lexy/input/string_input.hpp>

#include <lexy_ext/report_error.hpp>

namespace
{

namespace ast
{

    struct symbol
    {
        std::string              name;
        std::vector<cento::Rect> pads;
    };

    struct instance
    {
        std::string  symbol;
        cento::Point origin;
    };

    struct statement
    {
        std::variant<symbol, instance> v;

        template <typename T>
        constexpr statement(T&& t) : v(std::move(t))
        {}
    };

    struct plan
    {
        std::vector<symbol>   symbols;
        std::vector<instance> instances;
    };

}

namespace grammar
{

    namespace dsl = lexy::dsl;

    // An integer literal.
    // Supports decimal literals.
    struct integer : lexy::token_production
    {
        static constexpr auto rule  = dsl::minus_sign + dsl::integer<i32>;
        static constexpr auto value = lexy::as_integer<i32>;
    };

    // An string literal.
    // Supports alpha numeric characters only.
    struct string : lexy::token_production
    {
        struct invalid_char
        {
            static LEXY_CONSTEVAL auto name()
            {
                return "invalid character in string literal";
            }
        };

        static constexpr auto rule  = []()
        {
            auto code_point = dsl::unicode::alnum.error<invalid_char>;
            return dsl::quoted(code_point);
        }();
        static constexpr auto value = lexy::as_string<std::string, lexy::utf8_encoding>;
    };

    constexpr auto ws = dsl::whitespace(dsl::ascii::space);

    struct point
    {
        static constexpr auto whitespace = ws;
    
        static constexpr auto rule  = dsl::round_bracketed(dsl::times<2>(ws + dsl::p<integer> + ws));
        static constexpr auto value = lexy::construct<cento::Point>;
    };

    struct rect : lexy::token_production
    {
        static constexpr auto whitespace = ws;

        static constexpr auto rule  = dsl::round_bracketed(dsl::times<2>(ws + dsl::p<point> + ws));
        static constexpr auto value = lexy::construct<cento::Rect>;
    };

    // Comment.
    struct comment
    {
        static constexpr auto rule  = LEXY_LIT(";") >> dsl::until(dsl::ascii::newline);
        static constexpr auto value = lexy::forward<void>;
    };

    // We allow surrounding the document with comments and whitespace.
    // (Whitespace does not allow productions, so we need to inline it).
    constexpr auto ws_comment = ws | dsl::inline_<comment>;

    struct rects : lexy::transparent_production
    {
        static constexpr auto rule  = []()
        {
            return dsl::round_bracketed.opt_list(ws + dsl::p<rect> + ws);
        }();

        static constexpr auto value = lexy::as_list<std::vector<cento::Rect>>;
    };

    struct symbol : lexy::token_production
    {
        static constexpr auto whitespace = ws;

        static constexpr auto rule = []()
        {
            auto content = LEXY_LIT("symbol") +
                           ws +
                           dsl::p<string> +
                           ws +
                           dsl::p<rects> +
                           ws;

            return dsl::round_bracketed(content);
        }();

        static constexpr auto value = lexy::construct<ast::symbol>;
    };

    struct inst : lexy::token_production
    {
        static constexpr auto whitespace = ws;

        static constexpr auto rule = []()
        {
            auto content = LEXY_LIT("inst") +
                           ws +
                           dsl::p<string> +
                           ws +
                           dsl::p<point> +
                           ws;

            return dsl::round_bracketed(content);
        }();

        static constexpr auto value = lexy::construct<ast::instance>;
    };

    struct statement : lexy::transparent_production
    {
        static constexpr auto name = "statement";

        struct expected_statement
        {
            static LEXY_CONSTEVAL auto name()
            {
                return "expected a statement";
            }
        };

        static constexpr auto rule = []()
        {
            auto sym  = dsl::peek(LEXY_LIT("(symbol")) >> dsl::p<symbol>;
            auto i    = dsl::peek(LEXY_LIT("(inst")) >> dsl::p<inst>;
            return sym | i | dsl::error<expected_statement>;
        }();

        static constexpr auto value = lexy::construct<ast::statement>;
    };

    struct lisp
    {
        // Whitespace is a sequence of space, tab, carriage return, or newline.
        // Add your comment syntax here.
        static constexpr auto whitespace = ws;

        static constexpr auto rule = []()
        {
            auto at_eof = dsl::peek(dsl::eof);
            return ws_comment + dsl::terminator(at_eof).opt_list(ws_comment + dsl::p<statement>) + ws_comment;
        }();

        static constexpr auto value = lexy::as_list<std::vector<ast::statement>>;
    };

}

}

int testLisp()
{
    auto error = lexy_ext::report_error;

    int count = 0;
    if (not lexy::parse<grammar::point>(lexy::zstring_input(u8"(0 0)"), error)) { ++count; }
    if (not lexy::parse<grammar::point>(lexy::zstring_input(u8"(  0   0   )"), error)) { ++count; }
    if (not lexy::parse<grammar::point>(lexy::zstring_input(u8"(-110 -100)"), error)) { ++count; }

    if (not lexy::parse<grammar::string>(lexy::zstring_input(u8"\"helloWorld\""), error)) { ++count; }
    if (not lexy::parse<grammar::string>(lexy::zstring_input(u8"\"sym\""), error)) { ++count; }

    if (not lexy::parse<grammar::rect>(lexy::zstring_input(u8"((0 0) (100 100))"), error)) { ++count; }
    if (not lexy::parse<grammar::rect>(lexy::zstring_input(u8"( (100    0) (-100   100)   )"), error)) { ++count; }

    if (not lexy::parse<grammar::rect>(lexy::zstring_input(u8"((0 0) (100 100)) ((0 0) (100 100))"), error)) { ++count; }
    if (not lexy::parse<grammar::rect>(lexy::zstring_input(u8"((0 0) (100 100))((0 0) (100 100))((0 0) (100 100))"), error)) { ++count; }

    if (not lexy::parse<grammar::symbol>(lexy::zstring_input(u8"(symbol \"helloWorld\" (((0 0) (100 100))))"), error)) { ++count; }
    if (not lexy::parse<grammar::symbol>(lexy::zstring_input(u8"(symbol \"sym\" (((0 0) (100 100)) ((0 0) (100 100))))"), error)) { ++count; }

    if (not lexy::parse<grammar::inst>(lexy::zstring_input(u8"(inst \"helloWorld\" (0 0))"), error)) { ++count; }
    if (not lexy::parse<grammar::inst>(lexy::zstring_input(u8"(inst \"sym\" (100 100))"), error)) { ++count; }

    if (not lexy::parse<grammar::statement>(lexy::zstring_input(u8"(symbol \"helloWorld\" (((0 0) (100 100))))"), error)) { ++count; }
    if (not lexy::parse<grammar::statement>(lexy::zstring_input(u8"(inst \"helloWorld\" (0 0))"), error)) { ++count; }

    if (not lexy::parse<grammar::comment>(lexy::zstring_input(u8";test123\n"), error)) { ++count; }
    if (not lexy::parse<grammar::comment>(lexy::zstring_input(u8"; test asigfasd fasfdasd\n"), error)) { ++count; }

    if (count) { fmt::print("failed: {}\n", count); }

    return count;
}

std::vector<cento::Rect> parseLisp(const std::string_view path)
{
    auto file = lexy::read_file<lexy::utf8_encoding>(path.data());
    if (!file) { return {}; }

    const auto document = lexy::parse<grammar::lisp>(
        file.buffer(), lexy_ext::report_error.path(path.data()));
    if (!document) { return {}; }

    ast::plan plan;

    const std::vector<ast::statement>& statements = document.value();
    for (const ast::statement& s : statements)
    {
        std::visit([&](auto&& v)
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, ast::symbol>)
            {
                plan.symbols.push_back(v);
            }
            else if constexpr (std::is_same_v<T, ast::instance>)
            {
                plan.instances.push_back(v);
            }
            else
            {
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
            }
        }, s.v);
    }

    std::vector<cento::Rect> rects;
    for (const ast::symbol& sym : plan.symbols)
    {
        fmt::print("sym: \"{}\" pad count {}\n", sym.name, sym.pads.size());
    }
    for (const ast::instance& inst : plan.instances)
    {
        fmt::print("inst: \"{}\" ({}, {})\n", inst.symbol, inst.origin.x, inst.origin.y);
    }

    return rects;
}
