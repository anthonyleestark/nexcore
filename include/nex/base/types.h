/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   types.h
 * @brief  Fundamental type definitions and aliases for the Nex-ecosystem.
 * 
 * @details
 * This header defines fundamental type definitions and aliases that are used throughout the Nex-ecosystem.
 * These types include fixed-width integer types, pointer-sized integer types, floating-point types, character
 * types, boolean types, and pointer types. The definitions in this header are designed to provide a consistent
 * and portable set of types that can be used across different platforms and compilers, while also offering
 * short aliases for convenience and readability. The use of compiler built-ins and conditional compilation
 * ensures that the types are defined in a way that is optimal for the target environment, while also providing
 * fallbacks for cases where certain features may not be available.
 */

#include "nex/base/build.h"
#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/meta.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Standard fixed-width integer types
// =================================================================================

using int8      = signed char;              // 8-bit signed integer
using uint8     = unsigned char;            // 8-bit unsigned integer
using int16     = short;                    // 16-bit signed integer
using uint16    = unsigned short;           // 16-bit unsigned integer
using int32     = int;                      // 32-bit signed integer
using uint32    = unsigned int;             // 32-bit unsigned integer
using int64     = long long;                // 64-bit signed integer
using uint64    = unsigned long long;       // 64-bit unsigned integer

#if NEX_HAS_BUILTIN_INT128
    // Compilers that support 128-bit integers
    using int128    = __int128;             // 128-bit signed integer
    using uint128   = unsigned __int128;    // 128-bit unsigned integer
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Compile-time assertions to verify fixed-width integer type sizes
// =================================================================================

static_assert(sizeof(int8)   == 1, "Error: int8 must be 1 byte");
static_assert(sizeof(uint8)  == 1, "Error: uint8 must be 1 byte");
static_assert(sizeof(int16)  == 2, "Error: int16 must be 2 bytes");
static_assert(sizeof(uint16) == 2, "Error: uint16 must be 2 bytes");
static_assert(sizeof(int32)  == 4, "Error: int32 must be 4 bytes");
static_assert(sizeof(uint32) == 4, "Error: uint32 must be 4 bytes");
static_assert(sizeof(int64)  == 8, "Error: int64 must be 8 bytes");
static_assert(sizeof(uint64) == 8, "Error: uint64 must be 8 bytes");

#if NEX_HAS_BUILTIN_INT128
    static_assert(sizeof(int128)  == 16, "Error: int128 must be 16 bytes");
    static_assert(sizeof(uint128) == 16, "Error: uint128 must be 16 bytes");
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Short aliases for standard fixed-width integer types (Rust-style)
// =================================================================================

using i8        = int8;                     // 8-bit signed integer (int8)
using u8        = uint8;                    // 8-bit unsigned integer (uint8)
using i16       = int16;                    // 16-bit signed integer (int16)
using u16       = uint16;                   // 16-bit unsigned integer (uint16)
using i32       = int32;                    // 32-bit signed integer (int32)
using u32       = uint32;                   // 32-bit unsigned integer (uint32)
using i64       = int64;                    // 64-bit signed integer (int64)
using u64       = uint64;                   // 64-bit unsigned integer (uint64)

#if NEX_HAS_BUILTIN_INT128
    // Compilers that support 128-bit integers
    using i128  = int128;                   // 128-bit signed integer (int128)
    using u128  = uint128;                  // 128-bit unsigned integer (uint128)
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Literal suffixes for standard fixed-width integer types (Rust-style)
// =================================================================================

NEX_INLINE_NAMESPACE_BEGIN(literals)

constexpr i8 operator""_i8(uint64 value) noexcept   { return static_cast<i8>(value); }
constexpr u8 operator""_u8(uint64 value) noexcept   { return static_cast<u8>(value); }
constexpr i16 operator""_i16(uint64 value) noexcept { return static_cast<i16>(value); }
constexpr u16 operator""_u16(uint64 value) noexcept { return static_cast<u16>(value); }
constexpr i32 operator""_i32(uint64 value) noexcept { return static_cast<i32>(value); }
constexpr u32 operator""_u32(uint64 value) noexcept { return static_cast<u32>(value); }
constexpr i64 operator""_i64(uint64 value) noexcept { return static_cast<i64>(value); }
constexpr u64 operator""_u64(uint64 value) noexcept { return value; }  // no cast needed

#if NEX_HAS_BUILTIN_INT128
    /**
     * @note
     * For 128-bit integer literals, we provide both ways to cast:
     * - Directly from an unsigned 64-bit literal (for small values that fit in 64 bits)
     * - From a character sequence (for larger literals that exceed 64 bits)
     */

    constexpr i128 operator""_i128(uint64 value) noexcept { return static_cast<i128>(value); }
    constexpr u128 operator""_u128(uint64 value) noexcept { return static_cast<u128>(value); }

    template <char... Chars>
    constexpr i128 operator""_i128() noexcept {
        // Parse to unsigned first to avoid overflow issues with negative literals, then cast to signed
        return static_cast<i128>(meta::_parseRawInteger<u128, Chars...>()); 
    }

    template <char... Chars>
    constexpr u128 operator""_u128() noexcept { return meta::_parseRawInteger<u128, Chars...>(); }
#endif  // ^^NEX_HAS_BUILTIN_INT128

NEX_INLINE_NAMESPACE_END(literals)

// =================================================================================
// Macro definitions for fixed-width integer literal suffixes (C-style)
// =================================================================================

#define NEX_INT8_C(x)           (x ## _i8)
#define NEX_INT16_C(x)          (x ## _i16)
#define NEX_INT32_C(x)          (x ## _i32)
#define NEX_INT64_C(x)          (x ## _i64)

#define NEX_UINT8_C(x)          (x ## _u8)
#define NEX_UINT16_C(x)         (x ## _u16)
#define NEX_UINT32_C(x)         (x ## _u32)
#define NEX_UINT64_C(x)         (x ## _u64)

#if NEX_HAS_BUILTIN_INT128
    #define NEX_INT128_C(x)     (x ## _i128)
    #define NEX_UINT128_C(x)    (x ## _u128)
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Standard pointer-sized integer types
// =================================================================================

#if NEX_COMPILER_IS_MSVC
    #if NEX_BUILD_ENV_IS_64_BIT
        using intptr  = __int64;            // Pointer-sized signed integer (64-bit)
        using uintptr = unsigned __int64;   // Pointer-sized unsigned integer (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using intptr  = int;                // Pointer-sized signed integer (32-bit)
        using uintptr = unsigned int;       // Pointer-sized unsigned integer (32-bit)
    #endif  // ^^NEX_BUILD_ENV_IS_64_BIT
#elif defined(__INTPTR_TYPE__) && defined(__UINTPTR_TYPE__)
    // GCC, Clang, and other compatible compilers provide these types as built-in typedefs
    using intptr  = __INTPTR_TYPE__;        // Pointer-sized signed integer
    using uintptr = __UINTPTR_TYPE__;       // Pointer-sized unsigned integer
#elif NEX_BUILD_ENV_IS_64_BIT
    // 64-bit LP64 platforms (most Unix-like systems)
    using intptr  = long;                   // Pointer-sized signed integer
    using uintptr = unsigned long;          // Pointer-sized unsigned integer
#else  // Non-64-bit environment, assume 32-bit
    using intptr  = int;                    // Pointer-sized signed integer
    using uintptr = unsigned int;           // Pointer-sized unsigned integer
#endif  // ^^intptr/uintptr

// =================================================================================
// Compile-time assertions to verify pointer-sized integer type sizes
// =================================================================================

static_assert(sizeof(void*) == sizeof(intptr),  "Error: intptr must be the same size as a pointer");
static_assert(sizeof(void*) == sizeof(uintptr), "Error: uintptr must be the same size as a pointer");

// =================================================================================
// Short aliases for standard pointer-sized integer types (Rust-style)
// =================================================================================

using iptr  = intptr;                       // Pointer-sized signed integer (intptr)
using uptr  = uintptr;                      // Pointer-sized unsigned integer (uintptr)

// =================================================================================
// Standard maximum-width integer types
// =================================================================================

#if defined(__INTMAX_TYPE__) && defined(__UINTMAX_TYPE__)
    // GCC / Clang / ICC and other compatible compilers provide these types 
    // as built-in typedefs, and they always come together as a pair
    using intmax    = __INTMAX_TYPE__;      // Maximum-width signed integer
    using uintmax   = __UINTMAX_TYPE__;     // Maximum-width unsigned integer
#elif NEX_COMPILER_IS_MSVC
    using intmax    = __int64;              // Maximum-width signed integer
    using uintmax   = unsigned __int64;     // Maximum-width unsigned integer
#elif NEX_HAS_BUILTIN_INT128
    // Compilers with native 128-bit support
    using intmax    = int128;               // Maximum-width signed integer
    using uintmax   = uint128;              // Maximum-width unsigned integer
#else  // Safe fallback to longest available (long long is required by C++11+)
    using intmax    = long long;            // Maximum-width signed integer
    using uintmax   = unsigned long long;   // Maximum-width unsigned integer
#endif  // ^^intmax/uintmax

// =================================================================================
// Short aliases for standard maximum-width integer types (Rust-style)
// =================================================================================

using imax  = intmax;                       // Maximum-width signed integer (intmax)
using umax  = uintmax;                      // Maximum-width unsigned integer (uintmax)

// =================================================================================
// Standard minimum-width integer types
// =================================================================================

using int_least8    = int8;                 // Minimum 8-bit signed integer
using uint_least8   = uint8;                // Minimum 8-bit unsigned integer
using int_least16   = int16;                // Minimum 16-bit signed integer
using uint_least16  = uint16;               // Minimum 16-bit unsigned integer
using int_least32   = int32;                // Minimum 32-bit signed integer
using uint_least32  = uint32;               // Minimum 32-bit unsigned integer
using int_least64   = int64;                // Minimum 64-bit signed integer
using uint_least64  = uint64;               // Minimum 64-bit unsigned integer

#if NEX_HAS_BUILTIN_INT128
    // Compilers with native 128-bit support
    using int_least128   = int128;          // Minimum 128-bit signed integer
    using uint_least128  = uint128;         // Minimum 128-bit unsigned integer
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Standard fastest minimum-width integer types
// =================================================================================

using int_fast8     = int8;                 // Fastest minimum 8-bit signed integer
using uint_fast8    = uint8;                // Fastest minimum 8-bit unsigned integer
using int_fast16    = int16;                // Fastest minimum 16-bit signed integer
using uint_fast16   = uint16;               // Fastest minimum 16-bit unsigned integer
using int_fast32    = int32;                // Fastest minimum 32-bit signed integer
using uint_fast32   = uint32;               // Fastest minimum 32-bit unsigned integer
using int_fast64    = int64;                // Fastest minimum 64-bit signed integer
using uint_fast64   = uint64;               // Fastest minimum 64-bit unsigned integer

#if NEX_HAS_BUILTIN_INT128
    // Compilers with native 128-bit support
    using int_fast128    = int128;          // Fastest minimum 128-bit signed integer
    using uint_fast128   = uint128;         // Fastest minimum 128-bit unsigned integer
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Standard size, pointer difference and alignment types
// =================================================================================

#if NEX_COMPILER_IS_MSVC
    #if NEX_BUILD_ENV_IS_64_BIT
        using sizetype  = unsigned __int64;  // Represents sizes of objects in bytes, array lengths, etc.
        using ptrdiff   = __int64;           // Represents the difference between pointers in terms of array indexing
    #else  // Non-64-bit environment, assume 32-bit
        using sizetype  = unsigned int;      // Represents sizes of objects in bytes, array lengths, etc.
        using ptrdiff   = int;               // Represents the difference between pointers in terms of array indexing
    #endif  // ^^NEX_BUILD_ENV_IS_64_BIT
    using max_align = double;                // A POD type with the strictest alignment requirements
#else  // Non-MSVC compilers
    #if defined(__SIZE_TYPE__) && defined(__PTRDIFF_TYPE__)
        // GCC, Clang, and other compatible compilers provide these types as built-in typedefs
        using sizetype  = __SIZE_TYPE__;     // Represents sizes of objects in bytes, array lengths, etc.
        using ptrdiff   = __PTRDIFF_TYPE__;  // Represents the difference between pointers in terms of array indexing
    #elif NEX_BUILD_ENV_IS_64_BIT
        using sizetype  = unsigned long;     // Represents sizes of objects in bytes, array lengths, etc.
        using ptrdiff   = long;              // Represents the difference between pointers in terms of array indexing
    #else  // Non-64-bit environment, assume 32-bit
        using sizetype  = unsigned int;      // Represents sizes of objects in bytes, array lengths, etc.
        using ptrdiff   = int;               // Represents the difference between pointers in terms of array indexing
    #endif  // ^^sizetype/ptrdiff
    #if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
        // A POD type with the strictest alignment requirements
        using max_align = __attribute__((__aligned__)) long double;
    #else  // Safe fallback for other compilers
        using max_align = long double;       // A POD type with the strictest alignment requirements
    #endif  // ^^max_align
#endif  // ^^sizetype/ptrdiff/max_align

using usize     = sizetype;         // Represents sizes of objects in bytes, array lengths, etc.
using isize     = ptrdiff;          // Represents the difference between pointers in terms of array indexing

// =================================================================================
// Compile-time evaluations for size, pointer difference and alignment types
// =================================================================================

static_assert(sizeof(void*) == sizeof(sizetype),    "Error: sizetype must be the same size as a pointer");
static_assert(sizeof(void*) == sizeof(ptrdiff),     "Error: ptrdiff must be the same size as a pointer");
static_assert(alignof(void*) == alignof(max_align), "Error: max_align must have the strictest alignment requirements");

// =================================================================================
// Short aliases for standard size and pointer difference types (Rust-style)
// =================================================================================

using size      = usize;        // Represents sizes of objects in bytes, array lengths, etc. (usize)
using ssize     = isize;        // Represents the difference between pointers in terms of array indexing (isize)

// =================================================================================
// Literal suffixes for standard size and pointer difference types (Rust-style)
// =================================================================================

NEX_INLINE_NAMESPACE_BEGIN(literals)

constexpr usize operator""_uz(uint64 value) noexcept      { return static_cast<usize>(value); }
constexpr isize operator""_iz(uint64 value) noexcept      { return static_cast<isize>(value); }
constexpr max_align operator""_ma(ldouble value) noexcept { return static_cast<max_align>(value); }

NEX_INLINE_NAMESPACE_END(literals)

// ================================================================================
// Macro definitions for size and pointer difference literal suffixes (C-style)
// =================================================================================

#define NEX_SIZE_C(x)       (x ## _uz)
#define NEX_PTRDIFF_C(x)    (x ## _iz)
#define NEX_MAX_ALIGN_C(x)  (x ## _ma)

// =================================================================================
// Common power-of-two alignment types (for specifying alignment requirements)
// =================================================================================

using align1   = uint8;                           // 1-byte alignment
using align2   = uint16;                          // 2-byte alignment
using align4   = uint32;                          // 4-byte alignment
using align8   = uint64;                          // 8-byte alignment
using align16  = struct { uint64 a; uint64 b; };  // 16-byte alignment storage

// =================================================================================
// Convenient alternative names for common integer types (maybe platform-dependent)
// =================================================================================

using sshort    = int16;                // 16-bit signed integer
using ushort    = uint16;               // 16-bit unsigned integer
using sint      = int32;                // 32-bit signed integer
using uint      = uint32;               // 32-bit unsigned integer
using slong     = long;                 // 32 or 64-bit signed integer (platform-dependent)
using ulong     = unsigned long;        // 32 or 64-bit unsigned integer (platform-dependent)

using longlong  = int64;                // 64-bit signed integer
using ulonglong = uint64;               // 64-bit unsigned integer

// =================================================================================
// Short aliases for common integer types (Rust-style)
// =================================================================================

using ll        = longlong;             // 64-bit signed integer (long long)
using ull       = ulonglong;            // 64-bit unsigned integer (unsigned long long)

// =================================================================================
// Literal suffixes for common integer types (Rust-style)
// =================================================================================

NEX_INLINE_NAMESPACE_BEGIN(literals)

constexpr sshort operator""_ssh(uint64 value) noexcept     { return static_cast<sshort>(value); }
constexpr ushort operator""_ush(uint64 value) noexcept     { return static_cast<ushort>(value); }
constexpr sint operator""_si(uint64 value) noexcept        { return static_cast<sint>(value); }
constexpr uint operator""_ui(uint64 value) noexcept        { return static_cast<uint>(value); }
constexpr slong operator""_sl(uint64 value) noexcept       { return static_cast<slong>(value); }
constexpr ulong operator""_ul(uint64 value) noexcept       { return static_cast<ulong>(value); }
constexpr longlong operator""_ll(uint64 value) noexcept    { return static_cast<longlong>(value); }
constexpr ulonglong operator""_ull(uint64 value) noexcept  { return value; }  // no cast needed

NEX_INLINE_NAMESPACE_END(literals)

// =================================================================================
// Macro definitions for common integer literal suffixes (C-style)
// =================================================================================

#define NEX_SSHORT_C(x)     (x ## _ssh)
#define NEX_USHORT_C(x)     (x ## _ush)
#define NEX_SINT_C(x)       (x ## _si)
#define NEX_UINT_C(x)       (x ## _ui)
#define NEX_SLONG_C(x)      (x ## _sl)
#define NEX_ULONG_C(x)      (x ## _ul)
#define NEX_LONGLONG_C(x)   (x ## _ll)
#define NEX_ULONGLONG_C(x)  (x ## _ull)

// =================================================================================
// Standard floating-point types
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    // Compilers with native 16-bit floating-point type support
    using float16   = __float16_t;      // 16-bit half-precision IEEE 754 (binary16)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

using float32       = float;            // 32-bit single-precision IEEE 754 (binary32)
using float64       = double;           // 64-bit double-precision IEEE 754 (binary64)
using ldouble       = long double;      // Extended precision IEEE 754 (platform dependent)

#if NEX_HAS_BUILTIN_FLOAT128
    // Compilers with native 128-bit floating-point type support
    using float128  = __float128;       // 128-bit quadruple-precision IEEE 754 (binary128)
    using floatmax  = float128;         // Widest standard floating-point type
#else  // No support for 128-bit floating point
    using floatmax  = ldouble;          // Widest standard floating-point type
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Compile-time assertions to verify floating-point type sizes
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    static_assert(sizeof(float16) == 2, "Error: float16 must be 2 bytes");
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

static_assert(sizeof(float32) == 4, "Error: float32 must be 4 bytes");
static_assert(sizeof(float64) == 8, "Error: float64 must be 8 bytes");

#if NEX_HAS_BUILTIN_FLOAT128
    static_assert(sizeof(float128) == 16, "Error: float128 must be 16 bytes");
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Short aliases for standard floating-point types (Rust-style)
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    using f16   = float16;              // 16-bit floating point (IEEE 754 binary16) (float16)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

using f32       = float32;              // 32-bit floating point (IEEE 754 binary32) (float32)
using f64       = float64;              // 64-bit floating point (IEEE 754 binary64) (float64)

#if NEX_HAS_BUILTIN_FLOAT128
    using f128  = float128;             // 128-bit floating point (IEEE 754 binary128) (float128)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Literal suffixes for standard floating-point types (Rust-style)
// =================================================================================

NEX_INLINE_NAMESPACE_BEGIN(literals)

#if NEX_HAS_BUILTIN_FLOAT16
    constexpr f16 operator""_f16(ldouble value) noexcept { 
        // Note: This conversion may not be exact due to the limited precision of float16, 
        // but it allows for convenient literals.
        return static_cast<f16>(value); 
    }
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

constexpr f32 operator""_f32(ldouble value) noexcept    { return static_cast<f32>(value); }
constexpr f64 operator""_f64(ldouble value) noexcept    { return static_cast<f64>(value); }
constexpr ldouble operator""_ld(ldouble value) noexcept { return value; }  // no cast needed

#if NEX_HAS_BUILTIN_FLOAT128
    /**
     * @note
     * For 128-bit floating-point literals, we also provide both ways to cast:
     * - Directly from a long double literal (for values that fit within long double precision)
     * - From a character sequence (for larger literals that require full 128-bit precision)
     */

    constexpr f128 operator""_f128(ldouble value) noexcept { return static_cast<f128>(value); }

    #if NEX_HAS_BUILTIN(__strtof128)
        constexpr f128 operator""_f128(const char* str) noexcept { return __builtin_strtof128(str, nullptr); }
    #else  // Safe fallback for compilers without __strtof128 builtin support
        template <char... Chars>
        constexpr f128 operator""_f128() noexcept { return meta::_parseRawFloating<f128, Chars...>(); }
    #endif  // ^^NEX_HAS_BUILTIN(__strtof128)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

NEX_INLINE_NAMESPACE_END(literals)

// =================================================================================
// Macro definitions for standard floating-point literal suffixes (C-style)
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    #define NEX_FLOAT16_C(x)    (x ## _f16)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

#define NEX_FLOAT32_C(x)        (x ## _f32)
#define NEX_FLOAT64_C(x)        (x ## _f64)

#if NEX_HAS_BUILTIN_FLOAT128
    #define NEX_FLOAT128_C(x)   (x ## _f128)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Fixed-point types (typically used for graphics, audio, and multimedia)
// =================================================================================

using unorm8    = uint8;        // 8-bit unsigned normalized fixed-point ([0.0, 1.0] mapped to [0, 255])
using snorm8    = int8;         // 8-bit signed normalized fixed-point ([-1.0, 1.0] mapped to [-128, 127])
using unorm16   = uint16;       // 16-bit unsigned normalized fixed-point ([0.0, 1.0] mapped to [0, 65535])
using snorm16   = int16;        // 16-bit signed normalized fixed-point ([-1.0, 1.0] mapped to [-32768, 32767])

// =================================================================================
// Standard character and code unit types (encoding-aware; C++20)
// =================================================================================

#if NEX_HAS_BUILTIN_CHAR8_T
    using char8 = char8_t;                  // UTF-8 code unit (since C++20)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using char16    = char16_t;                 // UTF-16 code unit (since C++11)
using char32    = char32_t;                 // UTF-32 code unit (since C++11)
using nchar     = char;                     // Narrow/native character type (platform-dependent)
using wchar     = wchar_t;                  // Wide character type (platform-dependent)
using schar     = signed char;              // Signed byte / narrow character storage
using uchar     = unsigned char;            // Unsigned byte / narrow character storage
using codepoint = char32;                   // Preferred Unicode code point type (UCS-4 / UTF-32)

#if defined(__WINT_TYPE__)
    using wint  = __WINT_TYPE__;            // Wide integer type (for wide I/O functions)
#elif NEX_COMPILER_IS_MSVC
    using wint  = uint16;                   // Wide integer type (for wide I/O functions)
#else  // Safe fallback for other compilers
    using wint  = uint32;                   // Wide integer type (for wide I/O functions)
#endif  // ^^wint

// =================================================================================
// Short aliases for standard character types (Rust-style)
// =================================================================================

#if NEX_HAS_BUILTIN_CHAR8_T
    using c8    = char8;                    // UTF-8 code unit (since C++20)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using c16       = char16;                   // UTF-16 code unit (since C++11)
using c32       = char32;                   // UTF-32 code unit (since C++11)

// =================================================================================
// Literal suffixes for standard character types (Rust-style)
// =================================================================================

NEX_INLINE_NAMESPACE_BEGIN(literals)

#if NEX_HAS_BUILTIN_CHAR8_T
    constexpr c8 operator""_c8(char8 value) noexcept     { return value; }
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

constexpr c16 operator""_c16(char16 value) noexcept      { return value; }
constexpr c32 operator""_c32(char32 value) noexcept      { return value; }

#if NEX_HAS_BUILTIN_CHAR8_T
    constexpr c8 operator""_c8(nchar value) noexcept     { return static_cast<c8>(value); }
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

constexpr nchar operator""_nc(nchar value) noexcept      { return value; }
constexpr wchar operator""_wc(wchar value) noexcept      { return value; }
constexpr schar operator""_sc(nchar value) noexcept      { return static_cast<schar>(value); }
constexpr uchar operator""_uc(nchar value) noexcept      { return static_cast<uchar>(value); }
constexpr codepoint operator""_cp(char32 value) noexcept { return value; }

NEX_INLINE_NAMESPACE_END(literals)

// =================================================================================
// Macro definitions for standard character literal suffixes (C-style)
// =================================================================================

#if NEX_HAS_BUILTIN_CHAR8_T
    #define NEX_CHAR8_C(x)  (x ## _c8)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

#define NEX_CHAR16_C(x)     (x ## _c16)
#define NEX_CHAR32_C(x)     (x ## _c32)

#define NEX_NCHAR_C(x)      (x ## _nc)
#define NEX_WCHAR_C(x)      (x ## _wc)
#define NEX_SCHAR_C(x)      (x ## _sc)
#define NEX_UCHAR_C(x)      (x ## _uc)
#define NEX_CODEPOINT_C(x)  (x ## _cp)

// =================================================================================
// Standard boolean storage types
// =================================================================================

using boolean   = bool;                     // Boolean value (true or false)
using bool8     = uint8;                    // 8-bit boolean (0 = false, non-zero = true)
using bool16    = uint16;                   // 16-bit boolean (0 = false, non-zero = true)
using bool32    = uint32;                   // 32-bit boolean (0 = false, non-zero = true)

// =================================================================================
// Short aliases for boolean storage types (Rust-style)
// =================================================================================

using b8    = uint8;                        // 8-bit boolean (0 = false, non-zero = true)
using b16   = uint16;                       // 16-bit boolean (0 = false, non-zero = true)
using b32   = uint32;                       // 32-bit boolean (0 = false, non-zero = true)

// =================================================================================
// Common flags and bitfield types
// =================================================================================

using flags8     = uint8;                   // 8-bit flags (bitfield storage)
using flags16    = uint16;                  // 16-bit flags (bitfield storage)
using flags32    = uint32;                  // 32-bit flags (bitfield storage)
using flags64    = uint64;                  // 64-bit flags (bitfield storage)

using fflags8    = uint8;                   // 8-bit flags (bitfield storage, alternative name)
using fflags16   = uint16;                  // 16-bit flags (bitfield storage, alternative name)
using fflags32   = uint32;                  // 32-bit flags (bitfield storage, alternative name)
using fflags64   = uint64;                  // 64-bit flags (bitfield storage, alternative name)

// =================================================================================
// Standard pointer types and address types
// =================================================================================

using null_ptr  = decltype(nullptr);        // Null pointer type
enum class byte : uint8 {};                 // Single-byte object-representation type

/**
 * @note 
 * This section provides operator overloads for the `byte` type, allowing it to be used in bitwise operations 
 * and shifts, as well as a utility function to convert `byte` to an integral type. 
 * These functions are implemented based on the implementation of the Standard Library, using SFINAE to ensure 
 * they only participate in overload resolution when the appropriate conditions are met (e.g., when the shift 
 * amount is an integral type). 
 * The use of `static_cast` are intentional to ensure that the operations are performed correctly while maintaining 
 * type safety and avoiding unintended implicit conversions.
 */

NEX_INLINE_NAMESPACE_BEGIN(byte_operations)

// Bitwise left shift operator for 'byte' type
template <class IntType>
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr 
byte operator<<(const byte arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    if constexpr (meta::IsConstantEvaluated()) {
        static_assert(shift >= 0 && shift < 8, 
            "Error: Shift amount must be in range [0, 7] for byte type.");
    }
    return static_cast<byte>(static_cast<uint8>(
            static_cast<uint32>(static_cast<meta::UnderlyingTypeT<byte>>(arg)) << shift));
}

// Bitwise right shift operator for 'byte' type
template <class IntType>
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr 
byte operator>>(const byte arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    if constexpr (meta::IsConstantEvaluated()) {
        static_assert(shift >= 0 && shift < 8, 
            "Error: Shift amount must be in range [0, 7] for byte type.");
    }
    return static_cast<byte>(static_cast<uint8>(
            static_cast<uint32>(static_cast<meta::UnderlyingTypeT<byte>>(arg)) >> shift));
}

// Bitwise OR operator for 'byte' type
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr 
byte operator|(const byte left, const byte right) noexcept {
    return static_cast<byte>(
        static_cast<meta::UnderlyingTypeT<byte>>(left) | static_cast<meta::UnderlyingTypeT<byte>>(right));
}

// Bitwise AND operator for 'byte' type
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr 
byte operator&(const byte left, const byte right) noexcept {
    return static_cast<byte>(
        static_cast<meta::UnderlyingTypeT<byte>>(left) & static_cast<meta::UnderlyingTypeT<byte>>(right));
}

// Bitwise XOR operator for 'byte' type
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr 
byte operator^(const byte left, const byte right) noexcept {
    return static_cast<byte>(
        static_cast<meta::UnderlyingTypeT<byte>>(left) ^ static_cast<meta::UnderlyingTypeT<byte>>(right));
}

// Bitwise NOT operator for 'byte' type
NEX_NODISCARD NEX_ALWAYS_INLINE constexpr byte operator~(const byte arg) noexcept {
    return static_cast<byte>(~static_cast<meta::UnderlyingTypeT<byte>>(arg));
}

// Bitwise left shift compound assignment operator for 'byte' type
template <class IntType>
NEX_ALWAYS_INLINE constexpr byte& operator<<=(byte& arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return arg = arg << shift;
}

// Bitwise right shift compound assignment operator for 'byte' type
template <class IntType>
NEX_ALWAYS_INLINE constexpr byte& operator>>=(byte& arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return arg = arg >> shift;
}

// Bitwise OR compound assignment operator for 'byte' type
NEX_ALWAYS_INLINE constexpr byte& operator|=(byte& left, const byte right) noexcept {
    return left = left | right;
}

// Bitwise AND compound assignment operator for 'byte' type
NEX_ALWAYS_INLINE constexpr byte& operator&=(byte& left, const byte right) noexcept {
    return left = left & right;
}

// Bitwise XOR compound assignment operator for 'byte' type
NEX_ALWAYS_INLINE constexpr byte& operator^=(byte& left, const byte right) noexcept {
    return left = left ^ right;
}

// Convert 'byte' to an integral type
template <class IntType>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr IntType toInteger(const byte arg) noexcept
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return static_cast<IntType>(static_cast<meta::UnderlyingTypeT<byte>>(arg));
}

// Explicit conversion from 'byte' to 'bool'
template <class ByteType>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr bool toBool(const ByteType arg) noexcept 
requires (meta::IsSameV<ByteType, byte>) {
    return static_cast<meta::UnderlyingTypeT<byte>>(arg) != 0;
}

// Convert an integral type to 'byte' (symmetric to toInteger)
template <class IntType>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr byte toByte(IntType value) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return static_cast<byte>(static_cast<meta::UnderlyingTypeT<byte>>(value));
}

// Explicit conversion from 'bool' to 'byte'
template <class BoolType>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr byte toByte(BoolType value) noexcept
requires (meta::IsSameV<BoolType, bool>) {
    return static_cast<byte>(value ? 1u : 0u);
}

NEX_INLINE_NAMESPACE_END(byte_operations)

using raw_byte                  = uint8;                // Arithmetic byte type (8-bit unsigned integer)
using byte_ptr                  = raw_byte*;            // Pointer to arithmetic byte data
using const_byte_ptr            = const raw_byte*;      // Pointer to const-arithmetic byte data
using std_byte_ptr              = byte*;                // Pointer to byte data (std::byte equivalent)
using const_std_byte_ptr        = const byte*;          // Pointer to const-byte data (std::byte equivalent)
using void_ptr                  = void*;                // Pointer to void (generic pointer)
using const_void_ptr            = const void*;          // Pointer to const-void (generic pointer to const-data)
using address                   = uintptr;              // Represents a memory address as an unsigned integer (uintptr)
using byte_offset               = isize;                // Represents an offset in bytes (for pointer arithmetic) (isize)
using addr_offset               = isize;                // Represents an offset in bytes (for address arithmetic) (isize)
using handle                    = uintptr;              // Represents a generic handle or resource identifier (uintptr)

using volatile_void_ptr         = volatile void*;       // Generic pointer to volatile-data
using const_volatile_void_ptr   = const volatile void*; // Generic pointer to const-volatile data

// =================================================================================
// Short aliases for standard pointer types and address types (Rust-style)
// =================================================================================

using bptr       = byte_ptr;                    // Pointer to arithmetic byte data (byte_ptr)
using cbptr      = const_byte_ptr;              // Pointer to const-arithmetic byte data (const_byte_ptr)
using vptr       = void_ptr;                    // Pointer to void (generic pointer) (void_ptr)
using cvptr      = const_void_ptr;              // Pointer to const-void (generic pointer to const-data) (const_void_ptr)
using addr       = address;                     // Represents a memory address as an unsigned integer (uintptr)
using boff       = byte_offset;                 // Represents an offset in bytes (for pointer arithmetic) (byte_offset)
using aoff       = addr_offset;                 // Represents an offset in bytes (for address arithmetic) (addr_offset)
using hndl       = handle;                      // Represents a generic handle or resource identifier (handle)

using vvoid_ptr  = volatile_void_ptr;           // Generic pointer to volatile-data (volatile_void_ptr)
using cvvoid_ptr = const_volatile_void_ptr;     // Generic pointer to const-volatile data (const_volatile_void_ptr)

// =================================================================================
// Compile-time evaluations for byte type and related pointer types
// =================================================================================

static_assert(sizeof(byte) == 1,     "Error: byte must be exactly 1 byte in size");
static_assert(sizeof(raw_byte) == 1, "Error: raw_byte must be exactly 1 byte in size");

// =================================================================================
// Standard character pointer types (encoding-aware; C++20)
// =================================================================================

using nchar_ptr             = nchar*;           // Pointer to narrow/native char
using const_nchar_ptr       = const nchar*;     // Pointer to const narrow/native char

#if NEX_HAS_BUILTIN_CHAR8_T
    using char8_ptr         = char8*;           // Pointer to UTF-8 code unit
    using const_char8_ptr   = const char8*;     // Pointer to const UTF-8 code unit
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using char16_ptr            = char16*;          // Pointer to UTF-16 code unit
using const_char16_ptr      = const char16*;    // Pointer to const UTF-16 code unit
using char32_ptr            = char32*;          // Pointer to UTF-32 code unit
using const_char32_ptr      = const char32*;    // Pointer to const UTF-32 code unit
using wchar_ptr             = wchar*;           // Pointer to wide character
using const_wchar_ptr       = const wchar*;     // Pointer to const wide character

// =================================================================================
// Short aliases for standard character pointer types (Rust-style)
// =================================================================================

using ncptr         = nchar_ptr;                // Pointer to narrow/native char (nchar_ptr)
using cncptr        = const_nchar_ptr;          // Pointer to const narrow/native char (const_nchar_ptr)

#if NEX_HAS_BUILTIN_CHAR8_T
    using c8ptr     = char8_ptr;                // Pointer to UTF-8 code unit (char8_ptr)
    using cc8ptr    = const_char8_ptr;          // Pointer to const UTF-8 code unit (const_char8_ptr)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using c16ptr        = char16_ptr;               // Pointer to UTF-16 code unit (char16_ptr)
using cc16ptr       = const_char16_ptr;         // Pointer to const UTF-16 code unit (const_char16_ptr)
using c32ptr        = char32_ptr;               // Pointer to UTF-32 code unit (char32_ptr)
using cc32ptr       = const_char32_ptr;         // Pointer to const UTF-32 code unit (const_char32_ptr)
using wcptr         = wchar_ptr;                // Pointer to wide character (wchar_ptr)
using cwcptr        = const_wchar_ptr;          // Pointer to const wide character (const_wchar_ptr)

// =================================================================================
// Null-terminated string pointer types (C-style / zero-terminated)
// =================================================================================

using cstring       = const_nchar_ptr;          // Null-terminated narrow/native string

#if NEX_HAS_BUILTIN_CHAR8_T
    using u8cstring = const_char8_ptr;          // Null-terminated UTF-8 string
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using u16cstring    = const_char16_ptr;         // Null-terminated UTF-16 string
using u32cstring    = const_char32_ptr;         // Null-terminated UTF-32 string
using wcstring      = const_wchar_ptr;          // Null-terminated wide string

// =================================================================================
// Short aliases for null-terminated string pointer types (Rust-style)
// =================================================================================

using cstr          = cstring;                  // Null-terminated narrow/native string (cstring)

#if NEX_HAS_BUILTIN_CHAR8_T
    using u8cstr    = u8cstring;                // Null-terminated UTF-8 string (u8cstring)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

using u16cstr       = u16cstring;               // Null-terminated UTF-16 string (u16cstring)
using u32cstr       = u32cstring;               // Null-terminated UTF-32 string (u32cstring)
using wcstr         = wcstring;                 // Null-terminated wide string (wcstring)

// =================================================================================
// Macro definitions for null-terminated string literal prefixes (C-style)
// =================================================================================

#define NEX_CSTR_C(x)        (x)                // Null-terminated narrow/native string literal

#if NEX_HAS_BUILTIN_CHAR8_T
    #define NEX_U8CSTR_C(x)  (u8 ## x)          // Null-terminated UTF-8 string literal
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

#define NEX_U16CSTR_C(x)     (u16 ## x)         // Null-terminated UTF-16 string literal
#define NEX_U32CSTR_C(x)     (u32 ## x)         // Null-terminated UTF-32 string literal
#define NEX_WCSTR_C(x)       (L ## x)           // Null-terminated wide string literal

// =================================================================================
// Empty structs / tag types for various purposes
// =================================================================================

// Representing a "monostate" or "unit" type
struct monostate {};

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator==(monostate, monostate) noexcept { return true; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator!=(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator<(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator>(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator<=(monostate, monostate) noexcept { return true; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator>=(monostate, monostate) noexcept { return true; }

// In-place construction tag type 
// (for specifying in-place construction without additional data)
struct in_place_tag {
    explicit in_place_tag() = default;
};
inline constexpr in_place_tag in_place{};

// In-place construction tag type for a specific type 
// (for specifying in-place construction with type information)
template <class Type>
struct in_place_type : in_place_tag {
    explicit in_place_type() = default;
};

// In-place construction tag type for a specific index 
// (for specifying in-place construction with index information)
template <sizetype Index>
struct in_place_index : in_place_tag {
    explicit in_place_index() = default;
};

// Represents an unexpected value or error state
struct unexpected_type {};
inline constexpr unexpected_type unexpect{};

NEX_NAMESPACE_END
