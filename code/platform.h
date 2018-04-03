
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

#define CODE_BLOCK(...) do { __VA_ARGS__ } while(0)

#ifndef DEBUG_LEVEL
#   define DEBUG_LEVEL 1
#endif

#if defined(NDEBUG) || DEBUG_LEVEL <= 0
#   define __no_debug__
#endif

#ifdef __no_debug__
#   define debug(...)
#   define ASSERT(EXPR)
#   define __MARK__
#else
#   define debug(...) __debug__(__VA_ARGS__+0)
#   define __debug__(N) if((N) == 0 || (N) < (DEBUG_LEVEL))

#   include <stdio.h>
#   define __MARK__ __MARK__0(__FILE__, __LINE__)
#   define __MARK__0(F, L) CODE_BLOCK(         \
        fprintf(stderr, "MARK %s %d\n", F, L); \
    )

#   include <assert.h>
#   define ASSERT(EXPR) assert(EXPR)
#endif

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

#define CALL_MACRO(MACRO, ...) MACRO(__VA_ARGS__)

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

#define _CHECK(...) _CHECKa(__VA_ARGS__, 1,)
#define _CHECKa(X, N, ...) N

#define _NOT_ONE(X) _CHECK(CONCAT_(_NOT_ONE_, X))
#define _NOT_ONE_1 ~, 0,

#define _COMPL(X) CONCAT_(_COMPL_, X)
#define _COMPL_0 1
#define _COMPL_1 0

#define IS_ONE(X) _COMPL(_NOT_ONE(X))

#define CPP_DISPATCH(MACRO, MODE, ...)  \
    CPP_DISPATCHa(                      \
        CONCAT_(CPP_DISPATCH_, MODE),   \
        MACRO,                          \
        NUM_ARGS(__VA_ARGS__),          \
        __VA_ARGS__                     \
    )

#define CPP_DISPATCHa(NEXT, MACRO, N, ...) NEXT(MACRO, N, __VA_ARGS__)

#define CPP_DISPATCH_ENUM(MACRO, N, ...) \
    CPP_DISPATCH_ENUMa(CONCAT_(CONCAT_(MACRO, _), N), __VA_ARGS__)

#define CPP_DISPATCH_ENUMa(NEXT, ...) NEXT(__VA_ARGS__)

#define CPP_DISPATCH_MULTI(MACRO, N, ...)        \
    CPP_DISPATCH_MULTIa(                         \
        CONCAT_(CPP_DISPATCH_MULTI_, IS_ONE(N)), \
        MACRO,                                   \
        N,                                       \
        __VA_ARGS__                              \
    )

#define CPP_DISPATCH_MULTIa(NEXT, MACRO, N, ...) NEXT(MACRO, N, __VA_ARGS__)

#define CPP_DISPATCH_MULTI_0(MACRO, N, ...) \
    CPP_DISPATCH_MULTI_0a(CONCAT_(MACRO, _MULTI), N, __VA_ARGS__)

#define CPP_DISPATCH_MULTI_0a(NEXT, N, ...) NEXT(N, __VA_ARGS__)

#define CPP_DISPATCH_MULTI_1(MACRO, N, ...) \
    CPP_DISPATCH_MULTI_0a(CONCAT_(MACRO, _SINGLE), N, __VA_ARGS__)

#define CPP_DISPATCH_MULTI_1a(NEXT, N, ...) NEXT(N, __VA_ARGS__)

#define CONSTRUCTOR_DECL(...) CPP_DISPATCH(CONSTRUCTOR_DECL, MULTI, __VA_ARGS__)

#define CONSTRUCTOR_DECL_SINGLE(TYPE) \
    extern struct TYPE *CONCAT_(TYPE, _init)(struct TYPE *)

#define CONSTRUCTOR_DECL_MULTI(TYPE, ...) \
    extern struct TYPE *CONCAT_(TYPE, _init)(struct TYPE *, __VA_ARGS__)

#define DESTRUCTOR_DECL(TYPE) \
    extern struct TYPE *CONCAT_(TYPE, _finalize)(struct TYPE *)


#define CONSTRUCTOR_DEF(...) CPP_DISPATCH(CONSTRUCTOR_DEF, MULTI, __VA_ARGS__)

#define CONSTRUCTOR_DEF_SINGLE(TYPE) \
    struct TYPE *CONCAT_(TYPE, _init)(struct TYPE *self)

#define CONSTRUCTOR_DEF_MULTI(TYPE, ...) \
    struct TYPE *CONCAT_(TYPE, _init)(struct TYPE *self, __VA_ARGS__)

#define DESTRUCTOR_DEF(TYPE) \
    struct TYPE *CONCAT_(TYPE, _finalize)(struct TYPE *)

#define ALLOC(...) CPP_DISPATCH(ALLOC, ENUM, __VA_ARGS__)

#define ALLOC_1(OUT)              _ALLOC_(OUT,     1, sizeof(*(OUT)))
#define ALLOC_2(OUT, COUNT)       _ALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define ALLOC_3(OUT, COUNT, SIZE) _ALLOC_(OUT, COUNT,           SIZE)

#define _ALLOC_(OUT, COUNT, SIZE) \
    CODE_BLOCK(OUT = malloc((COUNT)*(SIZE));)


#define REALLOC(...) CPP_DISPATCH(REALLOC, ENUM, __VA_ARGS__)

#define REALLOC_1(OUT)              _REALLOC_(OUT,     1, sizeof(*(OUT)))
#define REALLOC_2(OUT, COUNT)       _REALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define REALLOC_3(OUT, COUNT, SIZE) _REALLOC_(OUT, COUNT,           SIZE)

#define _REALLOC_(OUT, COUNT, SIZE) \
    CODE_BLOCK(OUT = realloc(OUT, (COUNT)*(SIZE));)


#define CALLOC(...) CPP_DISPATCH(CALLOC, ENUM, __VA_ARGS__)

#define CALLOC_1(OUT)              _CALLOC_(OUT,     1, sizeof(*(OUT)))
#define CALLOC_2(OUT, COUNT)       _CALLOC_(OUT, COUNT, sizeof(*(OUT)))
#define CALLOC_3(OUT, COUNT, SIZE) _CALLOC_(OUT, COUNT,           SIZE)

#define _CALLOC_(OUT, COUNT, SIZE) \
    CODE_BLOCK(OUT = calloc((COUNT), (SIZE));)


#define COPY(...) CPP_DISPATCH(COPY, ENUM, __VA_ARGS__)

#define COPY_2(OUT, IN)              _COPY_(OUT, IN,     1, sizeof(*(OUT)))
#define COPY_3(OUT, IN, COUNT)       _COPY_(OUT, IN, COUNT, sizeof(*(OUT)))
#define COPY_4(OUT, IN, COUNT, SIZE) _COPY_(OUT, IN, COUNT,           SIZE)

#define _COPY_(OUT, IN, COUNT, SIZE) \
    CODE_BLOCK(memcpy((OUT), (IN), (COUNT)*(SIZE));)


#define MOVE(...) CPP_DISPATCH(MOVE, ENUM, __VA_ARGS__)

#define MOVE_2(OUT, IN)              _MOVE_(OUT, IN,     1, sizeof(*(OUT)))
#define MOVE_3(OUT, IN, COUNT)       _MOVE_(OUT, IN, COUNT, sizeof(*(OUT)))
#define MOVE_4(OUT, IN, COUNT, SIZE) _MOVE_(OUT, IN, COUNT,           SIZE)

#define _MOVE_(OUT, IN, COUNT, SIZE) \
    CODE_BLOCK(memmove((OUT), (IN), (COUNT)*(SIZE));)


#define CONCAT(...) CPP_DISPATCH(CONCAT, ENUM, __VA_ARGS__)
#define CONCAT_(A, B) CONCAT__(A, B)
#define CONCAT__(A, B) A##B

#define CONCAT_1(X) X
#define CONCAT_2(X, ...) CONCAT_(X, CONCAT_1(__VA_ARGS__))
#define CONCAT_3(X, ...) CONCAT_(X, CONCAT_2(__VA_ARGS__))
#define CONCAT_4(X, ...) CONCAT_(X, CONCAT_3(__VA_ARGS__))
#define CONCAT_5(X, ...) CONCAT_(X, CONCAT_4(__VA_ARGS__))
#define CONCAT_6(X, ...) CONCAT_(X, CONCAT_5(__VA_ARGS__))
#define CONCAT_7(X, ...) CONCAT_(X, CONCAT_6(__VA_ARGS__))
#define CONCAT_8(X, ...) CONCAT_(X, CONCAT_7(__VA_ARGS__))
#define CONCAT_9(X, ...) CONCAT_(X, CONCAT_8(__VA_ARGS__))

#define CONCAT_10(X, ...) CONCAT_(X, CONCAT_9(__VA_ARGS__))
#define CONCAT_11(X, ...) CONCAT_(X, CONCAT_10(__VA_ARGS__))
#define CONCAT_12(X, ...) CONCAT_(X, CONCAT_11(__VA_ARGS__))
#define CONCAT_13(X, ...) CONCAT_(X, CONCAT_12(__VA_ARGS__))
#define CONCAT_14(X, ...) CONCAT_(X, CONCAT_13(__VA_ARGS__))
#define CONCAT_15(X, ...) CONCAT_(X, CONCAT_14(__VA_ARGS__))
#define CONCAT_16(X, ...) CONCAT_(X, CONCAT_15(__VA_ARGS__))
#define CONCAT_17(X, ...) CONCAT_(X, CONCAT_16(__VA_ARGS__))
#define CONCAT_18(X, ...) CONCAT_(X, CONCAT_17(__VA_ARGS__))
#define CONCAT_19(X, ...) CONCAT_(X, CONCAT_18(__VA_ARGS__))

#define CONCAT_20(X, ...) CONCAT_(X, CONCAT_19(__VA_ARGS__))
#define CONCAT_21(X, ...) CONCAT_(X, CONCAT_20(__VA_ARGS__))
#define CONCAT_22(X, ...) CONCAT_(X, CONCAT_21(__VA_ARGS__))
#define CONCAT_23(X, ...) CONCAT_(X, CONCAT_22(__VA_ARGS__))
#define CONCAT_24(X, ...) CONCAT_(X, CONCAT_23(__VA_ARGS__))
#define CONCAT_25(X, ...) CONCAT_(X, CONCAT_24(__VA_ARGS__))
#define CONCAT_26(X, ...) CONCAT_(X, CONCAT_25(__VA_ARGS__))
#define CONCAT_27(X, ...) CONCAT_(X, CONCAT_26(__VA_ARGS__))
#define CONCAT_28(X, ...) CONCAT_(X, CONCAT_27(__VA_ARGS__))
#define CONCAT_29(X, ...) CONCAT_(X, CONCAT_28(__VA_ARGS__))

#define CONCAT_30(X, ...) CONCAT_(X, CONCAT_29(__VA_ARGS__))
#define CONCAT_31(X, ...) CONCAT_(X, CONCAT_30(__VA_ARGS__))
#define CONCAT_32(X, ...) CONCAT_(X, CONCAT_31(__VA_ARGS__))
#define CONCAT_33(X, ...) CONCAT_(X, CONCAT_32(__VA_ARGS__))
#define CONCAT_34(X, ...) CONCAT_(X, CONCAT_33(__VA_ARGS__))
#define CONCAT_35(X, ...) CONCAT_(X, CONCAT_34(__VA_ARGS__))
#define CONCAT_36(X, ...) CONCAT_(X, CONCAT_35(__VA_ARGS__))
#define CONCAT_37(X, ...) CONCAT_(X, CONCAT_36(__VA_ARGS__))
#define CONCAT_38(X, ...) CONCAT_(X, CONCAT_37(__VA_ARGS__))
#define CONCAT_39(X, ...) CONCAT_(X, CONCAT_38(__VA_ARGS__))

#define CONCAT_40(X, ...) CONCAT_(X, CONCAT_39(__VA_ARGS__))
#define CONCAT_41(X, ...) CONCAT_(X, CONCAT_40(__VA_ARGS__))
#define CONCAT_42(X, ...) CONCAT_(X, CONCAT_41(__VA_ARGS__))
#define CONCAT_43(X, ...) CONCAT_(X, CONCAT_42(__VA_ARGS__))
#define CONCAT_44(X, ...) CONCAT_(X, CONCAT_43(__VA_ARGS__))
#define CONCAT_45(X, ...) CONCAT_(X, CONCAT_44(__VA_ARGS__))
#define CONCAT_46(X, ...) CONCAT_(X, CONCAT_45(__VA_ARGS__))
#define CONCAT_47(X, ...) CONCAT_(X, CONCAT_46(__VA_ARGS__))
#define CONCAT_48(X, ...) CONCAT_(X, CONCAT_47(__VA_ARGS__))
#define CONCAT_49(X, ...) CONCAT_(X, CONCAT_48(__VA_ARGS__))

#define CONCAT_50(X, ...) CONCAT_(X, CONCAT_49(__VA_ARGS__))
#define CONCAT_51(X, ...) CONCAT_(X, CONCAT_50(__VA_ARGS__))
#define CONCAT_52(X, ...) CONCAT_(X, CONCAT_51(__VA_ARGS__))
#define CONCAT_53(X, ...) CONCAT_(X, CONCAT_52(__VA_ARGS__))
#define CONCAT_54(X, ...) CONCAT_(X, CONCAT_53(__VA_ARGS__))
#define CONCAT_55(X, ...) CONCAT_(X, CONCAT_54(__VA_ARGS__))
#define CONCAT_56(X, ...) CONCAT_(X, CONCAT_55(__VA_ARGS__))
#define CONCAT_57(X, ...) CONCAT_(X, CONCAT_56(__VA_ARGS__))
#define CONCAT_58(X, ...) CONCAT_(X, CONCAT_57(__VA_ARGS__))
#define CONCAT_59(X, ...) CONCAT_(X, CONCAT_58(__VA_ARGS__))

#define CONCAT_60(X, ...) CONCAT(X, CONCAT_59(__VA_ARGS__))
#define CONCAT_61(X, ...) CONCAT(X, CONCAT_60(__VA_ARGS__))
#define CONCAT_62(X, ...) CONCAT(X, CONCAT_61(__VA_ARGS__))
#define CONCAT_63(X, ...) CONCAT(X, CONCAT_62(__VA_ARGS__))

#define FREE(...) CODE_BLOCK(CPP_DISPATCH(FREE, ENUM, __VA_ARGS__))

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

