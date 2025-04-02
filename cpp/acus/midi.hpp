//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef midi_hpp
#define midi_hpp

#pragma once

#include <string_view>
#include <vector>

#include "cento/centoRect.hpp"

std::vector<cento::Rect> parseMidi(const std::string_view path);

#endif // midi_hpp
