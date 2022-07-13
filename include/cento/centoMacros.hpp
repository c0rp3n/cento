//
// Copyright (C) 2022 by Oliver John Hitchcock - github.com/c0rp3n
//

#ifndef centoMacros_hpp
#define centoMacros_hpp

#pragma once

// Faster than std::forward
#define CENTO_FWD(...) static_cast<decltype(__VA_ARGS__) &&>(__VA_ARGS__)

// Force a function to be inline
#if defined(CENTO_FWD_NO_FORCEINLINE)
#  define CENTO_FORCEINLINE inline
#else
#  if defined(_MSC_VER)
#    define CENTO_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
#    define CENTO_FORCEINLINE inline __attribute__((__always_inline__))
#  else
#    define CENTO_FORCEINLINE inline
#  endif
#endif

#endif // centoMacros_hpp
