//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef lisp_hpp
#define lisp_hpp

#pragma once

#include <string_view>
#include <vector>

#include "cento/centoRect.hpp"

int                      testLisp();
std::vector<cento::Rect> parseLisp(const std::string_view path);

#endif // lisp_hpp
