//
//  math_utils.h
//
//  Created by Volodymyr Shymanksyy on 8/10/19.
//  Copyright © 2019 Volodymyr Shymanskyy. All rights reserved.
//

#pragma once

#include "pico/m3/core.h"
#include <limits.h>

/*
 * Rotr, Rotl
 */

static inline u32 rotl32(u32 n, unsigned c)
{
    const unsigned mask = CHAR_BIT * sizeof(n) - 1;
    c &= mask & 31;
    return (n << c) | (n >> ((-c) & mask));
}

static inline u32 rotr32(u32 n, unsigned c)
{
    const unsigned mask = CHAR_BIT * sizeof(n) - 1;
    c &= mask & 31;
    return (n >> c) | (n << ((-c) & mask));
}

static inline u64 rotl64(u64 n, unsigned c)
{
    const unsigned mask = CHAR_BIT * sizeof(n) - 1;
    c &= mask & 63;
    return (n << c) | (n >> ((-c) & mask));
}

static inline u64 rotr64(u64 n, unsigned c)
{
    const unsigned mask = CHAR_BIT * sizeof(n) - 1;
    c &= mask & 63;
    return (n >> c) | (n << ((-c) & mask));
}

/*
 * Integer Div, Rem
 */

#define OP_DIV_U(RES, A, B)                \
    if (M3_UNLIKELY(B == 0))               \
        newTrap(m3Err_trapDivisionByZero); \
    RES = A / B;

#define OP_REM_U(RES, A, B)                \
    if (M3_UNLIKELY(B == 0))               \
        newTrap(m3Err_trapDivisionByZero); \
    RES = A % B;

// 2's complement detection
#if (INT_MIN != -INT_MAX)

#define OP_DIV_S(RES, A, B, TYPE_MIN)           \
    if (M3_UNLIKELY(B == 0))                    \
        newTrap(m3Err_trapDivisionByZero);      \
    if (M3_UNLIKELY(B == -1 and A == TYPE_MIN)) \
    {                                           \
        newTrap(m3Err_trapIntegerOverflow);     \
    }                                           \
    RES = A / B;

#define OP_REM_S(RES, A, B, TYPE_MIN)           \
    if (M3_UNLIKELY(B == 0))                    \
        newTrap(m3Err_trapDivisionByZero);      \
    if (M3_UNLIKELY(B == -1 and A == TYPE_MIN)) \
        RES = 0;                                \
    else                                        \
        RES = A % B;

#else

#define OP_DIV_S(RES, A, B, TYPE_MIN) OP_DIV_U(RES, A, B)
#define OP_REM_S(RES, A, B, TYPE_MIN) OP_REM_U(RES, A, B)

#endif

/*
 * Trunc
 */

#define OP_TRUNC(RES, A, TYPE, RMIN, RMAX)    \
    if (M3_UNLIKELY(isnan(A)))                \
    {                                         \
        newTrap(m3Err_trapIntegerConversion); \
    }                                         \
    if (M3_UNLIKELY(A <= RMIN or A >= RMAX))  \
    {                                         \
        newTrap(m3Err_trapIntegerOverflow);   \
    }                                         \
    RES = (TYPE)A;

#define OP_I32_TRUNC_F32(RES, A) OP_TRUNC(RES, A, i32, -2147483904.0f, 2147483648.0f)
#define OP_U32_TRUNC_F32(RES, A) OP_TRUNC(RES, A, u32, -1.0f, 4294967296.0f)
#define OP_I32_TRUNC_F64(RES, A) OP_TRUNC(RES, A, i32, -2147483649.0, 2147483648.0)
#define OP_U32_TRUNC_F64(RES, A) OP_TRUNC(RES, A, u32, -1.0, 4294967296.0)

#define OP_I64_TRUNC_F32(RES, A) OP_TRUNC(RES, A, i64, -9223373136366403584.0f, 9223372036854775808.0f)
#define OP_U64_TRUNC_F32(RES, A) OP_TRUNC(RES, A, u64, -1.0f, 18446744073709551616.0f)
#define OP_I64_TRUNC_F64(RES, A) OP_TRUNC(RES, A, i64, -9223372036854777856.0, 9223372036854775808.0)
#define OP_U64_TRUNC_F64(RES, A) OP_TRUNC(RES, A, u64, -1.0, 18446744073709551616.0)

#define OP_TRUNC_SAT(RES, A, TYPE, RMIN, RMAX, IMIN, IMAX) \
    if (M3_UNLIKELY(isnan(A)))                             \
    {                                                      \
        RES = 0;                                           \
    }                                                      \
    else if (M3_UNLIKELY(A <= RMIN))                       \
    {                                                      \
        RES = IMIN;                                        \
    }                                                      \
    else if (M3_UNLIKELY(A >= RMAX))                       \
    {                                                      \
        RES = IMAX;                                        \
    }                                                      \
    else                                                   \
    {                                                      \
        RES = (TYPE)A;                                     \
    }

#define OP_I32_TRUNC_SAT_F32(RES, A) OP_TRUNC_SAT(RES, A, i32, -2147483904.0f, 2147483648.0f, INT32_MIN, INT32_MAX)
#define OP_U32_TRUNC_SAT_F32(RES, A) OP_TRUNC_SAT(RES, A, u32, -1.0f, 4294967296.0f, 0UL, UINT32_MAX)
#define OP_I32_TRUNC_SAT_F64(RES, A) OP_TRUNC_SAT(RES, A, i32, -2147483649.0, 2147483648.0, INT32_MIN, INT32_MAX)
#define OP_U32_TRUNC_SAT_F64(RES, A) OP_TRUNC_SAT(RES, A, u32, -1.0, 4294967296.0, 0UL, UINT32_MAX)

#define OP_I64_TRUNC_SAT_F32(RES, A) OP_TRUNC_SAT(RES, A, i64, -9223373136366403584.0f, 9223372036854775808.0f, INT64_MIN, INT64_MAX)
#define OP_U64_TRUNC_SAT_F32(RES, A) OP_TRUNC_SAT(RES, A, u64, -1.0f, 18446744073709551616.0f, 0ULL, UINT64_MAX)
#define OP_I64_TRUNC_SAT_F64(RES, A) OP_TRUNC_SAT(RES, A, i64, -9223372036854777856.0, 9223372036854775808.0, INT64_MIN, INT64_MAX)
#define OP_U64_TRUNC_SAT_F64(RES, A) OP_TRUNC_SAT(RES, A, u64, -1.0, 18446744073709551616.0, 0ULL, UINT64_MAX)

/*
 * Min, Max
 */

#if d_m3HasFloat

#include <math.h>

static inline f32 min_f32(f32 a, f32 b)
{
    if (M3_UNLIKELY(isnan(a) or isnan(b)))
        return NAN;
    if (M3_UNLIKELY(a == 0 and a == b))
        return signbit(a) ? a : b;
    return a > b ? b : a;
}

static inline f32 max_f32(f32 a, f32 b)
{
    if (M3_UNLIKELY(isnan(a) or isnan(b)))
        return NAN;
    if (M3_UNLIKELY(a == 0 and a == b))
        return signbit(a) ? b : a;
    return a > b ? a : b;
}

static inline f64 min_f64(f64 a, f64 b)
{
    if (M3_UNLIKELY(isnan(a) or isnan(b)))
        return NAN;
    if (M3_UNLIKELY(a == 0 and a == b))
        return signbit(a) ? a : b;
    return a > b ? b : a;
}

static inline f64 max_f64(f64 a, f64 b)
{
    if (M3_UNLIKELY(isnan(a) or isnan(b)))
        return NAN;
    if (M3_UNLIKELY(a == 0 and a == b))
        return signbit(a) ? b : a;
    return a > b ? a : b;
}
#endif
