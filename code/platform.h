
#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

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

#define REVERSE_SEQUENCE_64() 63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48, \
                              47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32, \
                              31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16, \
                              15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, REVERSE_SEQUENCE_64())
#define NUM_ARGS_(...) NUM_ARGS__(__VA_ARGS__)
#define NUM_ARGS__( _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14, \
                   _15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28, \
                   _29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42, \
                   _43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56, \
                   _57,_58,_59,_60,_61,_62,_63,N,...) N

#define CONCAT(A, B) _CONCAT_(A, B)
#define _CONCAT_(A, B) A##B

#define CPP_DISPATCH(MACRO, ...) \
    _CPP_DISPATCH_(CONCAT(MACRO, CONCAT(_, NUM_ARGS(__VA_ARGS__))), __VA_ARGS__)

#define _CPP_DISPATCH_(MACRO, ...) __CPP_DISPATCH__(MACRO, __VA_ARGS__)
#define __CPP_DISPATCH__(MACRO, ...) MACRO(__VA_ARGS__)

#define ALLOC(...) CPP_DISPATCH(ALLOC, __VA_ARGS__)

#define ALLOC_1(OUT)              _ALLOC_(OUT,     1, sizeof(*(OUT)))
#define ALLOC_2(OUT, COUNT)       _ALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define ALLOC_3(OUT, COUNT, SIZE) _ALLOC_(OUT, COUNT,           SIZE)

#define _ALLOC_(OUT, COUNT, SIZE) \
    do { OUT = malloc((COUNT)*(SIZE)); } while(0)


#define REALLOC(...) CPP_DISPATCH(REALLOC, __VA_ARGS__)

#define REALLOC_1(OUT)              _REALLOC_(OUT,     1, sizeof(*(OUT)))
#define REALLOC_2(OUT, COUNT)       _REALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define REALLOC_3(OUT, COUNT, SIZE) _REALLOC_(OUT, COUNT,           SIZE)

#define _REALLOC_(OUT, COUNT, SIZE) \
    do { OUT = realloc(OUT, (COUNT)*(SIZE)); } while(0)


#define CALLOC(...) CPP_DISPATCH(CALLOC, __VA_ARGS__)

#define CALLOC_1(OUT)              _CALLOC_(OUT,     1, sizeof(*(OUT)))
#define CALLOC_2(OUT, COUNT)       _CALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define CALLOC_3(OUT, COUNT, SIZE) _CALLOC_(OUT, COUNT,           SIZE)

#define _CALLOC_(OUT, COUNT, SIZE) \
    do { OUT = calloc((COUNT), (SIZE)); } while(0)


#define COPY(...) CPP_DISPATCH(COPY, __VA_ARGS__)

#define COPY_2(OUT, IN)              _COPY_(OUT, IN,     1, sizeof(*(OUT)))
#define COPY_3(OUT, IN, COUNT)       _COPY_(OUT, IN, COUNT, sizeof(*(OUT)))
#define COPY_4(OUT, IN, COUNT, SIZE) _COPY_(OUT, IN, COUNT,           SIZE)

#define _COPY_(OUT, IN, COUNT, SIZE) \
    do { memcpy((OUT), (IN), (COUNT)*(SIZE)); } while(0)


#define MOVE(...) CPP_DISPATCH(MOVE, __VA_ARGS__)

#define MOVE_2(OUT, IN)              _MOVE_(OUT, IN,     1, sizeof(*(OUT)))
#define MOVE_3(OUT, IN, COUNT)       _MOVE_(OUT, IN, COUNT, sizeof(*(OUT)))
#define MOVE_4(OUT, IN, COUNT, SIZE) _MOVE_(OUT, IN, COUNT,           SIZE)

#define _MOVE_(OUT, IN, COUNT, SIZE) \
    do { memmove((OUT), (IN), (COUNT)*(SIZE)); } while(0)


#define FREE(...) do { CPP_DISPATCH(FREE, __VA_ARGS__) } while(0)

#define FREE_1(X) free(X);
#define FREE_2(X, ...) free(X); FREE_1(__VA_ARGS__)
#define FREE_3(X, ...) free(X); FREE_2(__VA_ARGS__)
#define FREE_4(X, ...) free(X); FREE_3(__VA_ARGS__)
#define FREE_5(X, ...) free(X); FREE_4(__VA_ARGS__)
#define FREE_6(X, ...) free(X); FREE_5(__VA_ARGS__)
#define FREE_7(X, ...) free(X); FREE_6(__VA_ARGS__)
#define FREE_8(X, ...) free(X); FREE_7(__VA_ARGS__)
#define FREE_9(X, ...) free(X); FREE_8(__VA_ARGS__)

#define FREE_10(X, ...) free(X); FREE_9(__VA_ARGS__)
#define FREE_11(X, ...) free(X); FREE_10(__VA_ARGS__)
#define FREE_12(X, ...) free(X); FREE_11(__VA_ARGS__)
#define FREE_13(X, ...) free(X); FREE_12(__VA_ARGS__)
#define FREE_14(X, ...) free(X); FREE_13(__VA_ARGS__)
#define FREE_15(X, ...) free(X); FREE_14(__VA_ARGS__)
#define FREE_16(X, ...) free(X); FREE_15(__VA_ARGS__)
#define FREE_17(X, ...) free(X); FREE_16(__VA_ARGS__)
#define FREE_18(X, ...) free(X); FREE_17(__VA_ARGS__)
#define FREE_19(X, ...) free(X); FREE_18(__VA_ARGS__)

#define FREE_20(X, ...) free(X); FREE_19(__VA_ARGS__)
#define FREE_21(X, ...) free(X); FREE_20(__VA_ARGS__)
#define FREE_22(X, ...) free(X); FREE_21(__VA_ARGS__)
#define FREE_23(X, ...) free(X); FREE_22(__VA_ARGS__)
#define FREE_24(X, ...) free(X); FREE_23(__VA_ARGS__)
#define FREE_25(X, ...) free(X); FREE_24(__VA_ARGS__)
#define FREE_26(X, ...) free(X); FREE_25(__VA_ARGS__)
#define FREE_27(X, ...) free(X); FREE_26(__VA_ARGS__)
#define FREE_28(X, ...) free(X); FREE_27(__VA_ARGS__)
#define FREE_29(X, ...) free(X); FREE_28(__VA_ARGS__)

#define FREE_30(X, ...) free(X); FREE_29(__VA_ARGS__)
#define FREE_31(X, ...) free(X); FREE_30(__VA_ARGS__)
#define FREE_32(X, ...) free(X); FREE_31(__VA_ARGS__)
#define FREE_33(X, ...) free(X); FREE_32(__VA_ARGS__)
#define FREE_34(X, ...) free(X); FREE_33(__VA_ARGS__)
#define FREE_35(X, ...) free(X); FREE_34(__VA_ARGS__)
#define FREE_36(X, ...) free(X); FREE_35(__VA_ARGS__)
#define FREE_37(X, ...) free(X); FREE_36(__VA_ARGS__)
#define FREE_38(X, ...) free(X); FREE_37(__VA_ARGS__)
#define FREE_39(X, ...) free(X); FREE_38(__VA_ARGS__)

#define FREE_40(X, ...) free(X); FREE_39(__VA_ARGS__)
#define FREE_41(X, ...) free(X); FREE_40(__VA_ARGS__)
#define FREE_42(X, ...) free(X); FREE_41(__VA_ARGS__)
#define FREE_43(X, ...) free(X); FREE_42(__VA_ARGS__)
#define FREE_44(X, ...) free(X); FREE_43(__VA_ARGS__)
#define FREE_45(X, ...) free(X); FREE_44(__VA_ARGS__)
#define FREE_46(X, ...) free(X); FREE_45(__VA_ARGS__)
#define FREE_47(X, ...) free(X); FREE_46(__VA_ARGS__)
#define FREE_48(X, ...) free(X); FREE_47(__VA_ARGS__)
#define FREE_49(X, ...) free(X); FREE_48(__VA_ARGS__)

#define FREE_50(X, ...) free(X); FREE_49(__VA_ARGS__)
#define FREE_51(X, ...) free(X); FREE_50(__VA_ARGS__)
#define FREE_52(X, ...) free(X); FREE_51(__VA_ARGS__)
#define FREE_53(X, ...) free(X); FREE_52(__VA_ARGS__)
#define FREE_54(X, ...) free(X); FREE_53(__VA_ARGS__)
#define FREE_55(X, ...) free(X); FREE_54(__VA_ARGS__)
#define FREE_56(X, ...) free(X); FREE_55(__VA_ARGS__)
#define FREE_57(X, ...) free(X); FREE_56(__VA_ARGS__)
#define FREE_58(X, ...) free(X); FREE_57(__VA_ARGS__)
#define FREE_59(X, ...) free(X); FREE_58(__VA_ARGS__)

#define FREE_60(X, ...) free(X); FREE_59(__VA_ARGS__)
#define FREE_61(X, ...) free(X); FREE_60(__VA_ARGS__)
#define FREE_62(X, ...) free(X); FREE_61(__VA_ARGS__)
#define FREE_63(X, ...) free(X); FREE_62(__VA_ARGS__)

#endif /* !_PLATFORM_H */

