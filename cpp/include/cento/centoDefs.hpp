//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//
// Distributed under the Boost Software License, Version 1.0.
//

#ifndef centoDefs_hpp
#define centoDefs_hpp

#pragma once

#include <cstddef>
#include <cstdint>

using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;
using isize = std::ptrdiff_t;

using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;
using usize = std::size_t;

using f32 = float;
using f64 = double;

#define CENTO_MAKE_VERSION(major, minor, patch) \
    ((((u32)(major)) << 22) | (((u32)(minor)) << 12) | ((u32)(patch)))

constexpr const u32 CENTO_VERSION = CENTO_MAKE_VERSION(0, 1, 0);

#endif // centoDefs_hpp
