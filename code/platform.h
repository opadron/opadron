
#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef __int128_t i128;
typedef   int64_t  i64;
typedef   int32_t  i32;
typedef   int16_t  i16;
typedef   int8_t   i8;

typedef __uint128_t u128;
typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;

typedef  long double f128;
typedef  double      f64;
typedef  float       f32;

typedef size_t    counter;
typedef ptrdiff_t diff;

typedef void *ptr;
typedef ptr  *hdl;

/* typedef char char; */
typedef unsigned char uchar;
typedef signed char schar;

/* variables: file scope */
#define global
#define local static

/* variables: function scope */
/* define not-a-word */
#define persist static

/* functions: file scope */
/* define extern extern */
#define intern static

#ifndef DEBUG_LEVEL
#    define DEBUG_LEVEL 0
#endif

#if defined(NDEBUG) || DEBUG_LEVEL <= 0
#    define __no_debug__
#endif

#define debug(...) __debug__(__VA_ARGS__+0)
#define __debug__(N) if((N) == 0 || (N) < (DEBUG_LEVEL))

#define segfault (*((hdl)NULL) = NULL)

#define MAIN intern void __main__(int, char **);         \
             int main(int argc, char **argv) {           \
                 __main__(argc, argv);                   \
                 return 0;                               \
             }                                           \
             intern void __main__(int argc, char **argv)

#define RUN_ONCE_OR_RETURN RUN_ONCE_OR_RETURN_(___, __LINE__)
#define RUN_ONCE_OR_RETURN_(A, B) RUN_ONCE_OR_RETURN__(A##B)
#define RUN_ONCE_OR_RETURN__(VAR) persist u8 VAR = 0; \
                                  if(VAR == 0) ++VAR; \
                                  else return

#define ALLOC(...) ALLOC_(__VA_ARGS__, 1)
#define ALLOC_(VAR, COUNT, ...)               \
    do {                                      \
        VAR = malloc((COUNT)*sizeof(*(VAR))); \
    } while(0)

#define REALLOC(VAR, COUNT)                      \
    do {                                         \
        VAR = realloc(VAR, (COUNT)*sizeof(VAR)); \
    } while(0)

#endif /* !_PLATFORM_H */

