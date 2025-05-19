#pragma once

#define INLINE __attribute__((always_inline)) inline
#define HOT __attribute__((hot))
#define CONSTANT __attribute__((const))

#if defined(__GNUC__) && __GNUC__ >= 7 || defined(__clang__) && __clang_major__ >= 12
 #define FALLTHROUGH __attribute__ ((fallthrough))
#else
 #define FALLTHROUGH ((void)0)
#endif 