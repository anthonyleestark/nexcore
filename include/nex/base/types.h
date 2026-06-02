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
// Compiler-specific type definitions and feature detection
// =================================================================================

#if defined(__SIZEOF_INT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_INT128 1
#else // Compiler does not support __int128
    #define NEX_HAS_BUILTIN_INT128 0
#endif  // defined(__SIZEOF_INT128__) && !NEX_COMPILER_MSVC_COMPATIBLE

#if defined(__FLT16_DIG__) || defined(__fp16) || defined(_Float16)
    #define NEX_HAS_BUILTIN_FLOAT16 1
#else  // Compiler does not support __float16
    #define NEX_HAS_BUILTIN_FLOAT16 0
#endif  // defined(__FLT16_DIG__) || defined(__fp16) || defined(_Float16)

#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else  // Compiler does not support __float128
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif  // defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE

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

using ushort        = unsigned short;       // 16-bit unsigned integer (alternative name)
using uint          = unsigned int;         // 32-bit unsigned integer (alternative name)
using ulong         = unsigned long;        // 32 or 64-bit unsigned integer (alternative name, platform-dependent)

using longlong      = long long;            // 64-bit signed integer (alternative name)
using ulonglong     = unsigned long long;   // 64-bit unsigned integer (alternative name)

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

using ll        = longlong;                 // 64-bit signed integer (long long)
using ull       = ulonglong;                // 64-bit unsigned integer (unsigned long long)

// =================================================================================
// Literal suffixes for standard fixed-width integer types (Rust-style)
// =================================================================================

constexpr i8 operator""_i8(ulonglong value) noexcept   { return static_cast<i8>(value); }
constexpr u8 operator""_u8(ulonglong value) noexcept   { return static_cast<u8>(value); }
constexpr i16 operator""_i16(ulonglong value) noexcept { return static_cast<i16>(value); }
constexpr u16 operator""_u16(ulonglong value) noexcept { return static_cast<u16>(value); }
constexpr i32 operator""_i32(ulonglong value) noexcept { return static_cast<i32>(value); }
constexpr u32 operator""_u32(ulonglong value) noexcept { return static_cast<u32>(value); }
constexpr i64 operator""_i64(ulonglong value) noexcept { return static_cast<i64>(value); }
constexpr u64 operator""_u64(ulonglong value) noexcept { return static_cast<u64>(value); }

#if NEX_HAS_BUILTIN_INT128
    constexpr i128 operator""_i128(ulonglong value) noexcept { return static_cast<i128>(value); }
    constexpr u128 operator""_u128(ulonglong value) noexcept { return static_cast<u128>(value); }
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Macro definitions for fixed-width integer literal suffixes (C-style)
// =================================================================================

#define NEX_INT8_C(x)       (x ## _i8)
#define NEX_INT16_C(x)      (x ## _i16)
#define NEX_INT32_C(x)      (x ## _i32)
#define NEX_INT64_C(x)      (x ## _i64)

#define NEX_UINT8_C(x)      (x ## _u8)
#define NEX_UINT16_C(x)     (x ## _u16)
#define NEX_UINT32_C(x)     (x ## _u32)
#define NEX_UINT64_C(x)     (x ## _u64)

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

static_assert(sizeof(void*) == sizeof(intptr), "Error: intptr must be the same size as a pointer");
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

using size          = usize;        // Represents sizes of objects in bytes, array lengths, etc. (usize)
using ssize         = isize;        // Represents the difference between pointers in terms of array indexing (isize)

// =================================================================================
// Literal suffixes for standard size and pointer difference types (Rust-style)
// =================================================================================

constexpr usize operator""_uz(ulonglong value) noexcept { return static_cast<usize>(value); }
constexpr isize operator""_iz(ulonglong value) noexcept { return static_cast<isize>(value); }

// ================================================================================
// Macro definitions for size and pointer difference literal suffixes (C-style)
// =================================================================================

#define NEX_SIZE_C(x)      (x ## _uz)
#define NEX_PTRDIFF_C(x)   (x ## _iz)

// =================================================================================
// Common power-of-two alignment types (for specifying alignment requirements)
// =================================================================================

using align1   = byte;                            // 1-byte alignment
using align2   = uint16;                          // 2-byte alignment
using align4   = uint32;                          // 4-byte alignment
using align8   = uint64;                          // 8-byte alignment
using align16  = struct { uint64 a; uint64 b; };  // 16-byte alignment storage

// =================================================================================
// Standard floating-point types
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    #if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
        using float16  = __fp16;        // 16-bit floating point (IEEE 754 binary16)
    #elif defined(_Float16)
        using float16  = _Float16;      // 16-bit floating point (IEEE 754 binary16)
    #else  // Safe fallback for other compilers
        using float16  = uint16;        // 16-bit floating point (IEEE 754 binary16) represented as raw bits (uint16)
    #endif
#else  // Compiler does not support __float16
    using float16      = uint16;        // 16-bit floating point (IEEE 754 binary16) represented as raw bits (uint16)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

using float32       = float;            // 32-bit floating point (IEEE 754 binary32)
using float64       = double;           // 64-bit floating point (IEEE 754 binary64)
using ldouble       = long double;      // Extended precision floating point (platform dependent)

#if NEX_HAS_BUILTIN_FLOAT128
    using float128  = __float128;       // 128-bit floating point (IEEE 754 binary128)
    using floatmax  = float128;         // Widest standard floating-point type
#else  // No support for 128-bit floating point
    using floatmax  = ldouble;          // Widest standard floating-point type
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Compile-time assertions to verify floating-point type sizes
// =================================================================================

static_assert(sizeof(float16) == 2, "Error: float16 must be 2 bytes");
static_assert(sizeof(float32) == 4, "Error: float32 must be 4 bytes");
static_assert(sizeof(float64) == 8, "Error: float64 must be 8 bytes");

#if NEX_HAS_BUILTIN_FLOAT128
    static_assert(sizeof(float128) == 16, "Error: float128 must be 16 bytes");
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Short aliases for standard floating-point types (Rust-style)
// =================================================================================

using f16       = float16;              // 16-bit floating point (IEEE 754 binary16) (float16)
using f32       = float32;              // 32-bit floating point (IEEE 754 binary32) (float32)
using f64       = float64;              // 64-bit floating point (IEEE 754 binary64) (float64)

#if NEX_HAS_BUILTIN_FLOAT128
    using f128  = float128;             // 128-bit floating point (IEEE 754 binary128) (float128)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Literal suffixes for standard floating-point types (Rust-style)
// =================================================================================

constexpr f16 operator""_f16(ldouble value) noexcept { 
    // Note: This conversion may not be exact due to the limited precision of float16, 
    // but it allows for convenient literals.
    return static_cast<f16>(value); 
}
constexpr f32 operator""_f32(ldouble value) noexcept { return static_cast<f32>(value); }
constexpr f64 operator""_f64(ldouble value) noexcept { return static_cast<f64>(value); }

#if NEX_HAS_BUILTIN_FLOAT128
    constexpr f128 operator""_f128(ldouble value) noexcept { return static_cast<f128>(value); }
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Macro definitions for standard floating-point literal suffixes (C-style)
// =================================================================================

#define NEX_FLOAT16_C(x)        (x ## _f16)
#define NEX_FLOAT32_C(x)        (x ## _f32)
#define NEX_FLOAT64_C(x)        (x ## _f64)

#if NEX_HAS_BUILTIN_FLOAT128
    #define NEX_FLOAT128_C(x)   (x ## _f128)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Standard character and code unit types (encoding-aware; C++20)
// =================================================================================

using char8     = char8_t;                  // UTF-8 code unit (since C++20)
using char16    = char16_t;                 // UTF-16 code unit (since C++11)
using char32    = char32_t;                 // UTF-32 code unit (since C++11)
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

using c8        = char8;                    // UTF-8 code unit (since C++20)
using c16       = char16;                   // UTF-16 code unit (since C++11)
using c32       = char32;                   // UTF-32 code unit (since C++11)

// =================================================================================
// Literal suffixes for standard character types (Rust-style)
// =================================================================================

constexpr c8 operator""_c8(char8 value) noexcept    { return value; }
constexpr c16 operator""_c16(char16 value) noexcept { return value; }
constexpr c32 operator""_c32(char32 value) noexcept { return value; }

constexpr c8 operator""_c8(char value) noexcept     { return static_cast<c8>(value); }
constexpr wchar operator""_wc(wchar value) noexcept { return value; }

// =================================================================================
// Macro definitions for standard character literal suffixes (C-style)
// =================================================================================

#define NEX_CHAR8_C(x)     (x ## _c8)
#define NEX_CHAR16_C(x)    (x ## _c16)
#define NEX_CHAR32_C(x)    (x ## _c32)
#define NEX_WCHAR_C(x)     (x ## _wc)

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
enum class byte : unsigned char {};         // Single-byte object-representation type

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

// Get the underlying unsigned char value of a byte (C++23 std::to_underlying equivalent)
NEX_NODISCARD constexpr unsigned char toUnderlying(const byte arg) noexcept {
    return static_cast<unsigned char>(arg);
}

// Bitwise left shift operator for 'byte' type
template <class IntType>
NEX_NODISCARD constexpr byte operator<<(const byte arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    if constexpr (meta::IsConstantEvaluated()) {
        static_assert(shift >= 0 && shift < 8, 
            "Error: Shift amount must be in range [0, 7] for byte type.");
    }
    return static_cast<byte>(static_cast<unsigned char>(
            static_cast<unsigned int>(toUnderlying(arg)) << shift));
}

// Bitwise right shift operator for 'byte' type
template <class IntType>
NEX_NODISCARD constexpr byte operator>>(const byte arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    if constexpr (meta::IsConstantEvaluated()) {
        static_assert(shift >= 0 && shift < 8, 
            "Error: Shift amount must be in range [0, 7] for byte type.");
    }
    return static_cast<byte>(static_cast<unsigned char>(
            static_cast<unsigned int>(toUnderlying(arg)) >> shift));
}

// Bitwise OR operator for 'byte' type
NEX_NODISCARD constexpr byte operator|(const byte left, const byte right) noexcept {
    return static_cast<byte>(
            static_cast<unsigned char>(toUnderlying(left) | toUnderlying(right)));
}

// Bitwise AND operator for 'byte' type
NEX_NODISCARD constexpr byte operator&(const byte left, const byte right) noexcept {
    return static_cast<byte>(
        static_cast<unsigned char>(toUnderlying(left) & toUnderlying(right)));
}

// Bitwise XOR operator for 'byte' type
NEX_NODISCARD constexpr byte operator^(const byte left, const byte right) noexcept {
    return static_cast<byte>(
        static_cast<unsigned char>(toUnderlying(left) ^ toUnderlying(right)));
}

// Bitwise NOT operator for 'byte' type
NEX_NODISCARD constexpr byte operator~(const byte arg) noexcept {
    return static_cast<byte>(
        static_cast<unsigned char>(~static_cast<unsigned int>(toUnderlying(arg))));
}

// Bitwise left shift compound assignment operator for 'byte' type
template <class IntType>
constexpr byte& operator<<=(byte& arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return arg = arg << shift;
}

// Bitwise right shift compound assignment operator for 'byte' type
template <class IntType>
constexpr byte& operator>>=(byte& arg, const IntType shift) noexcept 
requires (meta::IsIntegralV<IntType> && !meta::IsSameV<IntType, bool>) {
    return arg = arg >> shift;
}

// Bitwise OR compound assignment operator for 'byte' type
constexpr byte& operator|=(byte& left, const byte right) noexcept {
    return left = left | right;
}

// Bitwise AND compound assignment operator for 'byte' type
constexpr byte& operator&=(byte& left, const byte right) noexcept {
    return left = left & right;
}

// Bitwise XOR compound assignment operator for 'byte' type
constexpr byte& operator^=(byte& left, const byte right) noexcept {
    return left = left ^ right;
}

// Convert 'byte' to an integral type
template <class IntType>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr IntType toInteger(const byte arg) noexcept
requires (meta::IsIntegralV<IntType> || !meta::IsSameV<IntType, bool>) {
    return static_cast<IntType>(toUnderlying(arg));
}

// Explicit conversion from 'byte' to 'bool'
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr bool toBool(const byte arg) noexcept {
    return toUnderlying(arg) != 0;
}

// Convert an integral type to 'byte' (symmetric to toInteger)
template <class IntType>
NEX_NODISCARD constexpr byte toByte(IntType value) noexcept 
requires (meta::IsIntegralV<IntType> || !meta::IsSameV<IntType, bool>) {
    return static_cast<byte>(static_cast<unsigned char>(value));
}

// Explicit conversion from 'bool' to 'byte'
template <class BoolType>
NEX_NODISCARD constexpr byte toByte(BoolType value) noexcept
requires (meta::IsSameV<BoolType, bool>) {
    return static_cast<byte>(value ? 1u : 0u);
}

using raw_byte              = uint8;            // Arithmetic byte type (8-bit unsigned integer)
using byte_ptr              = raw_byte*;        // Pointer to arithmetic byte data
using const_byte_ptr        = const raw_byte*;  // Pointer to const-arithmetic byte data
using std_byte_ptr          = byte*;            // Pointer to byte data (std::byte equivalent)
using const_std_byte_ptr    = const byte*;      // Pointer to const-byte data (std::byte equivalent)
using void_ptr              = void*;            // Pointer to void (generic pointer)
using const_void_ptr        = const void*;      // Pointer to const-void (generic pointer to const-data)
using address               = uintptr;          // Represents a memory address as an unsigned integer (uintptr)
using byte_offset           = isize;            // Represents an offset in bytes (for pointer arithmetic) (isize)
using handle                = uintptr;          // Represents a generic handle or resource identifier (uintptr)

using volatile_void_ptr         = volatile void*;        // Generic pointer to volatile-data
using const_volatile_void_ptr   = const volatile void*;  // Generic pointer to const-volatile data

// =================================================================================
// Compile-time evaluations for byte type and related pointer types
// =================================================================================

static_assert(sizeof(byte) == 1,     "Error: byte must be exactly 1 byte in size");
static_assert(sizeof(raw_byte) == 1, "Error: raw_byte must be exactly 1 byte in size");

// =================================================================================
// Standard character pointer types (encoding-aware; C++20)
// =================================================================================

using char_ptr              = char*;            // Pointer to char
using const_char_ptr        = const char*;      // Pointer to const char
using char8_ptr             = char8*;           // Pointer to UTF-8 code unit
using const_char8_ptr       = const char8*;     // Pointer to const UTF-8 code unit
using char16_ptr            = char16*;          // Pointer to UTF-16 code unit
using const_char16_ptr      = const char16*;    // Pointer to const UTF-16 code unit
using char32_ptr            = char32*;          // Pointer to UTF-32 code unit
using const_char32_ptr      = const char32*;    // Pointer to const UTF-32 code unit
using wchar_ptr             = wchar*;           // Pointer to wide character
using const_wchar_ptr       = const wchar*;     // Pointer to const wide character

// =================================================================================
// Null-terminated string pointer types (C-style / zero-terminated)
// =================================================================================

using cstring       = const_char_ptr;           // Null-terminated char string
using u8cstring     = const_char8_ptr;          // Null-terminated UTF-8 string
using u16cstring    = const_char16_ptr;         // Null-terminated UTF-16 string
using u32cstring    = const_char32_ptr;         // Null-terminated UTF-32 string
using wcstring      = const_wchar_ptr;          // Null-terminated wide string

// =================================================================================
// Short aliases for null-terminated string pointer types (Rust-style)
// =================================================================================

using cstr      = cstring;                      // Null-terminated char string (cstring)
using u8cstr    = u8cstring;                    // Null-terminated UTF-8 string (u8cstring)
using u16cstr   = u16cstring;                   // Null-terminated UTF-16 string (u16cstring)
using u32cstr   = u32cstring;                   // Null-terminated UTF-32 string (u32cstring)
using wcstr     = wcstring;                     // Null-terminated wide string (wcstring)

NEX_NAMESPACE_END
