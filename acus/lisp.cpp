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
        statement(T t) : v(std::move(t))
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
        static constexpr auto rule  = dsl::integer<i32>;
        static constexpr auto value = lexy::forward<i32>;
    };

    // An string literal.
    // Supports alpha numeric characters only.
    struct string : lexy::token_production
    {
        static constexpr auto rule  = []() { return dsl::quoted(dsl::unicode::alnum); };
        static constexpr auto value = lexy::as_string<std::string, lexy::utf8_encoding>;
    };

    constexpr auto ws = dsl::whitespace(dsl::ascii::blank / dsl::ascii::newline);

    struct point : lexy::token_production
    {
        static constexpr auto rule  = []()
        {
            return dsl::round_bracketed(dsl::times<2>(ws + dsl::p<integer> + ws));
        };

        static constexpr auto value = lexy::construct<cento::Point>;
    };

    struct rect : lexy::token_production
    {
        static constexpr auto rule  = []()
        {
            return dsl::round_bracketed(dsl::times<4>(ws + dsl::p<integer> + ws));
        };

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
    constexpr auto ws_comment = ws | dsl::inline_<comment> ;

    struct rects : lexy::transparent_production
    {
        static constexpr auto rule  = []()
        {
            return dsl::round_bracketed.opt_list(ws + dsl::p<rect> + ws);
        };

        static constexpr auto value = lexy::as_list<std::vector<cento::Rect>>;
    };

    struct symbol
    {
        static constexpr auto rule = []()
        {
            auto content = LEXY_LIT("symbol") +
                           ws +
                           dsl::p<string> +
                           ws +
                           dsl::p<rects> +
                           ws;

            return ws + dsl::round_bracketed(content);
        };

        static constexpr auto value = lexy::construct<ast::symbol>;
    };

    struct inst
    {
        static constexpr auto rule = []()
        {
            auto content = LEXY_LIT("inst") +
                           ws +
                           dsl::p<string> +
                           ws +
                           dsl::p<point> +
                           ws;

            return dsl::round_bracketed(content);
        };

        static constexpr auto value = lexy::construct<ast::instance>;
    };

    struct statement : lexy::transparent_production
    {
        struct expected_statement
        {
            static LEXY_CONSTEVAL auto name()
            {
                return "expected a statement";
            }
        };

        static constexpr auto rule = []()
        {
            return dsl::p<symbol> | dsl::p<inst> | dsl::error<expected_statement>;
        };

        static constexpr auto value = lexy::construct<ast::statement>;
    };

    struct lisp
    {
        struct as_plan
        {
            using return_type = ast::plan;

            constexpr ast::plan operator()(lexy::nullopt&&) const
            {
                return ast::plan();
            }

            template <typename... Args>
            constexpr auto operator()(Args&&... args) const
            {
                return ast::plan();
            }

            struct plan_sink
            {
                ast::plan plan;

                constexpr void operator()(ast::symbol&& sym)
                {
                    plan.symbols.emplace_back(LEXY_FWD(sym));
                }

                constexpr void operator()(ast::instance&& inst)
                {
                    plan.instances.emplace_back(LEXY_FWD(inst));
                }

                constexpr ast::plan&& finish() &&
                {
                    return LEXY_MOV(plan);
                }
            };

            constexpr auto sink() const
            {
                return plan_sink{};
            }
        };

        // Whitespace is a sequence of space, tab, carriage return, or newline.
        // Add your comment syntax here.
        static constexpr auto whitespace = ws;

        static constexpr auto rule = []()
        {
            auto at_eof = dsl::peek(dsl::eof);
            return ws_comment + dsl::terminator(at_eof).opt_list(dsl::p<statement>);
        };

        static constexpr auto value = as_plan{};
    };

}

}

std::vector<cento::Rect> parseLisp(const std::string_view path)
{
    auto file = lexy::read_file<lexy::utf8_encoding>(path.data());
    if (!file) { return {}; }

    const auto document = lexy::parse<grammar::lisp>(
        file.buffer(), lexy_ext::report_error.path(path.data()));
    if (!document) { return {}; }

    std::vector<cento::Rect> rects;
    const ast::plan& plan = document.value();
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
