/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  types.h
 * @brief Common type definitions and aliases for global use across the Nex-ecosystem.
 * 
 * @details
 * This header defines fundamental type aliases, including fixed-width integers, pointer-sized integers, 
 * size types, floating-point types, character types, boolean storage types, pointer types, and address types. 
 * These types are designed to provide consistent and descriptive type names across the Nex-ecosystem, while 
 * also adhering to C++20 standards and best practices for type safety and clarity, also providing Rust-style 
 * short aliases for convenience and readability.
 */

#include "nex/base/build.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/intrinsics.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Include standard fixed-width integer types
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
    using int128    = __int128;             // 128-bit signed integer
    using uint128   = unsigned __int128;    // 128-bit unsigned integer
#else
    using int128    = int64;                // No support for 128-bit signed integer, fallback to 64-bit
    using uint128   = uint64;               // No support for 128-bit unsigned integer, fallback to 64-bit
#endif

using ushort        = unsigned short;       // 16-bit unsigned integer (alternative name)
using uint          = unsigned int;         // 32-bit unsigned integer (alternative name)
using ulong         = unsigned long;        // 32 or 64-bit unsigned integer (alternative name, platform-dependent)

using longlong      = int64;                // 64-bit signed integer (int64)
using ulonglong     = uint64;               // 64-bit unsigned integer (uint64)

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
using i128      = int128;                   // 128-bit signed integer (int128)
using u128      = uint128;                  // 128-bit unsigned integer (uint128)

using ll        = longlong;                 // 64-bit signed integer (long long)
using ull       = ulonglong;                // 64-bit unsigned integer (unsigned long long)

// =================================================================================
// Include standard pointer-sized integer types
// =================================================================================

#if NEX_COMPILER_IS_MSVC
    #if NEX_BUILD_ENV_IS_64_BIT
        using intptr  = __int64;            // Pointer-sized signed integer (64-bit)
        using uintptr = unsigned __int64;   // Pointer-sized unsigned integer (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using intptr  = int;                // Pointer-sized signed integer (32-bit)
        using uintptr = unsigned int;       // Pointer-sized unsigned integer (32-bit)
    #endif
#else  // Non-MSVC compilers
    #if defined(__INTPTR_TYPE__) && defined(__UINTPTR_TYPE__)
        using intptr  = __INTPTR_TYPE__;    // Pointer-sized signed integer
        using uintptr = __UINTPTR_TYPE__;   // Pointer-sized unsigned integer
    #elif NEX_BUILD_ENV_IS_64_BIT
        using intptr  = long;               // Pointer-sized signed integer
        using uintptr = unsigned long;      // Pointer-sized unsigned integer
    #else  // Non-64-bit environment, assume 32-bit
        using intptr  = int;               // Pointer-sized signed integer
        using uintptr = unsigned int;      // Pointer-sized unsigned integer
    #endif
#endif

// =================================================================================
// Short aliases for standard pointer-sized integer types (Rust-style)
// =================================================================================

using iptr  = intptr;                       // Pointer-sized signed integer (intptr)
using uptr  = uintptr;                      // Pointer-sized unsigned integer (uintptr)

// =================================================================================
// Include standard maximum-width integer types
// =================================================================================

#if defined(__INTMAX_TYPE__)
    using intmax    = __INTMAX_TYPE__;          // Maximum-width signed integer
#elif NEX_COMPILER_IS_MSVC
    using intmax    = __int64;                  // Maximum-width signed integer
#elif NEX_HAS_BUILTIN_INT128
    using intmax    = int128;                   // Maximum-width signed integer
#else
    using intmax    = long long;                // Maximum-width signed integer
#endif

#if defined(__UINTMAX_TYPE__)
    using uintmax   = __UINTMAX_TYPE__;         // Maximum-width unsigned integer
#elif NEX_COMPILER_IS_MSVC
    using uintmax   = unsigned __int64;         // Maximum-width unsigned integer
#elif NEX_HAS_BUILTIN_INT128
    using uintmax   = uint128;                  // Maximum-width unsigned integer
#else
    using uintmax   = unsigned long long;       // Maximum-width unsigned integer
#endif

// =================================================================================
// Short aliases for standard maximum-width integer types (Rust-style)
// =================================================================================

using imax  = intmax;                       // Maximum-width signed integer (intmax)
using umax  = uintmax;                      // Maximum-width unsigned integer (uintmax)

// =================================================================================
// Include standard minimum-width integer types
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
    using int_least128   = int128;          // Minimum 128-bit signed integer
    using uint_least128  = uint128;         // Minimum 128-bit unsigned integer
#else
    using int_least128   = int_least64;     // No support for 128-bit signed integer, fallback to 64-bit
    using uint_least128  = uint_least64;    // No support for 128-bit unsigned integer, fallback to 64-bit
#endif

// =================================================================================
// Include standard fastest minimum-width integer types
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
    using int_fast128    = int128;          // Fastest minimum 128-bit signed integer
    using uint_fast128   = uint128;         // Fastest minimum 128-bit unsigned integer
#else
    using int_fast128    = int_fast64;      // No support for 128-bit signed integer, fallback to 64-bit
    using uint_fast128   = uint_fast64;     // No support for 128-bit unsigned integer, fallback to 64-bit
#endif

// =================================================================================
// Include standard size types
// =================================================================================

#if NEX_COMPILER_IS_MSVC
    #if NEX_BUILD_ENV_IS_64_BIT
        using sizetype  = unsigned __int64; // Unsigned integer type used for sizes & array indexing (64-bit)
        using ptrdiff   = __int64;          // Signed integer type used for pointer arithmetic & array indexing (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using sizetype  = unsigned int;     // Unsigned integer type used for sizes & array indexing (32-bit)
        using ptrdiff   = int;              // Signed integer type used for pointer arithmetic & array indexing (32-bit)
    #endif
    using max_align = double;               // Type with alignment at least as strict as any scalar type
#else
    #if defined(__SIZE_TYPE__) && defined(__PTRDIFF_TYPE__)
        using sizetype  = __SIZE_TYPE__;    // Unsigned integer type used for sizes & array indexing
        using ptrdiff   = __PTRDIFF_TYPE__; // Signed integer type used for pointer arithmetic & array indexing
    #elif NEX_BUILD_ENV_IS_64_BIT
        using sizetype  = unsigned long;    // Unsigned integer type used for sizes & array indexing (64-bit)
        using ptrdiff   = long;             // Signed integer type used for pointer arithmetic & array indexing (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using sizetype  = unsigned int;     // Unsigned integer type used for sizes & array indexing (32-bit)
        using ptrdiff   = int;              // Signed integer type used for pointer arithmetic & array indexing (32-bit)
    #endif
    using max_align = long double;          // Type with alignment at least as strict as any scalar type
#endif

using usize     = sizetype;                 // Unsigned integer type used for sizes & array indexing
using isize     = ptrdiff;                  // Signed integer type used for pointer arithmetic & array indexing

// =================================================================================
// Short aliases for standard size types (Rust-style)
// =================================================================================

using size          = usize;        // Unsigned integer type used for sizes & array indexing (usize)
using ssize         = isize;        // Signed integer type used for pointer arithmetic & array indexing (isize)

// =================================================================================
// Include standard floating-point types
// =================================================================================

using float32       = float;                // 32-bit floating point
using float64       = double;               // 64-bit floating point
using ldouble       = long double;          // Extended precision floating point
using floatmax      = long double;          // Widest standard floating-point type

#if NEX_HAS_BUILTIN_FLOAT128
    using float128  = __float128;           // 128-bit floating point
#else
    using float128  = floatmax;             // Widest built-in floating-point fallback
#endif

// =================================================================================
// Short aliases for standard floating-point types (Rust-style)
// =================================================================================

using f32       = float32;                  // 32-bit floating point (float32)
using f64       = float64;                  // 64-bit floating point (float64)
using f128      = float128;                 // 128-bit floating point (float128)

// =================================================================================
// Include standard character types (encoding-aware; C++20)
// =================================================================================

using char8     = char8_t;                  // 8-bit character type (UTF-8)
using char16    = char16_t;                 // 16-bit character type (UTF-16)
using char32    = char32_t;                 // 32-bit character type (UTF-32)
using wchar     = wchar_t;                  // Wide character type
using schar     = signed char;              // Signed narrow character storage type
using uchar     = unsigned char;            // Unsigned narrow character storage type
using codepoint = char32;                   // Unicode code point storage type

#if !defined(__WINT_TYPE__)
    using wint  = __WINT_TYPE__;            // Wide character type for I/O functions
#else
    using wint  = uint16;                   // Wide character type for I/O functions
#endif

// =================================================================================
// Short aliases for standard character types (Rust-style)
// =================================================================================

using c8        = char8;                    // 8-bit character type (UTF-8)
using c16       = char16;                   // 16-bit character type (UTF-16)
using c32       = char32;                   // 32-bit character type (UTF-32)

// =================================================================================
// Include boolean types (using uint8 and uint32 for boolean storage)
// =================================================================================

using boolean   = bool;                     // Boolean value (true or false)
using bool8     = uint8;                    // 8-bit boolean storage value (0 = false, non-zero = true)
using bool16    = uint16;                   // 16-bit boolean storage value (0 = false, non-zero = true)
using bool32    = uint32;                   // 32-bit boolean storage value (0 = false, non-zero = true)

// =================================================================================
// Short aliases for boolean types (Rust-style)
// =================================================================================

using b8    = uint8;                        // 8-bit boolean storage value (0 = false, non-zero = true)
using b16   = uint16;                       // 16-bit boolean storage value (0 = false, non-zero = true)
using b32   = uint32;                       // 32-bit boolean storage value (0 = false, non-zero = true)

// =================================================================================
// Include pointer and address-related types
// =================================================================================

using null_ptr  = decltype(nullptr);            // Null pointer type
enum class byte : unsigned char {};             // Single-byte object-representation type

/**
 * @brief Helper functions for 'byte' type operations
 * @details 
 * This section provides operator overloads for the `byte` type, allowing it to be used in bitwise 
 * operations and shifts, as well as a utility function to convert `byte` to an integral type. 
 * These functions are implemented using SFINAE to ensure they only participate in overload resolution 
 * when the appropriate conditions are met (e.g., when the shift amount is an integral type). 
 * The use of `static_cast` ensures that the operations are performed correctly while maintaining type safety.
 */

// Bitwise left shift operator for 'byte' type
template <class IntType, type_traits::EnableIfT<type_traits::IsIntegralV<IntType>, int> = 0>
NEX_NODISCARD constexpr byte operator<<(const byte arg, const IntType shift) noexcept {
    return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(arg) << shift));
}

// Bitwise right shift operator for 'byte' type
template <class IntType, type_traits::EnableIfT<type_traits::IsIntegralV<IntType>, int> = 0>
NEX_NODISCARD constexpr byte operator>>(const byte arg, const IntType shift) noexcept {
    // every static_cast is intentional
    return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(arg) >> shift));
}

// Bitwise OR operator for 'byte' type
NEX_NODISCARD constexpr byte operator|(const byte left, const byte right) noexcept {
    // every static_cast is intentional
    return static_cast<byte>(
        static_cast<unsigned char>(static_cast<unsigned int>(left) | static_cast<unsigned int>(right)));
}

// Bitwise AND operator for 'byte' type
NEX_NODISCARD constexpr byte operator&(const byte left, const byte right) noexcept {
    // every static_cast is intentional
    return static_cast<byte>(
        static_cast<unsigned char>(static_cast<unsigned int>(left) & static_cast<unsigned int>(right)));
}

// Bitwise XOR operator for 'byte' type
NEX_NODISCARD constexpr byte operator^(const byte left, const byte right) noexcept {
    // every static_cast is intentional
    return static_cast<byte>(
        static_cast<unsigned char>(static_cast<unsigned int>(left) ^ static_cast<unsigned int>(right)));
}

// Bitwise NOT operator for 'byte' type
NEX_NODISCARD constexpr byte operator~(const byte arg) noexcept {
    // every static_cast is intentional
    return static_cast<byte>(static_cast<unsigned char>(~static_cast<unsigned int>(arg)));
}

// Bitwise left shift compound assignment operator for 'byte' type
template <class IntType, type_traits::EnableIfT<type_traits::IsIntegralV<IntType>, int> = 0>
constexpr byte& operator<<=(byte& arg, const IntType shift) noexcept {
    return arg = arg << shift;
}

// Bitwise right shift compound assignment operator for 'byte' type
template <class IntType, type_traits::EnableIfT<type_traits::IsIntegralV<IntType>, int> = 0>
constexpr byte& operator>>=(byte& arg, const IntType shift) noexcept {
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

// Utility function to convert 'byte' to an integral type
template <class IntType, type_traits::EnableIfT<type_traits::IsIntegralV<IntType>, int> = 0>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr IntType toInteger(const byte arg) noexcept {
    return static_cast<IntType>(arg);
}

using raw_byte              = uint8;                // Arithmetic byte type
using byte_ptr              = raw_byte*;            // Pointer to arithmetic byte data
using const_byte_ptr        = const raw_byte*;      // Pointer to const arithmetic byte data
using std_byte_ptr          = byte*;                // Pointer to byte data
using const_std_byte_ptr    = const byte*;          // Pointer to const byte data
using void_ptr              = void*;                // Pointer to void
using const_void_ptr        = const void*;          // Pointer to const void
using address               = uintptr;              // Memory address (pointer-sized unsigned integer)

// =================================================================================
// Include character pointer types (encoding-aware; C++20)
// =================================================================================

using char_ptr              = char*;                // Pointer to char
using const_char_ptr        = const char*;          // Pointer to const char
using char8_ptr             = char8*;               // Pointer to char8
using const_char8_ptr       = const char8*;         // Pointer to const char8
using char16_ptr            = char16*;              // Pointer to char16
using const_char16_ptr      = const char16*;        // Pointer to const char16
using char32_ptr            = char32*;              // Pointer to char32
using const_char32_ptr      = const char32*;        // Pointer to const char32
using wchar_ptr             = wchar_t*;             // Pointer to wide character
using const_wchar_ptr       = const wchar_t*;       // Pointer to const wide character

// =================================================================================
// Include C-style null-terminated string types (encoding-aware; C++20)
// =================================================================================

using cstring           = const_char_ptr;           // Null-terminated const char string
using utf8_cstring      = const_char8_ptr;          // Null-terminated const UTF-8 string
using utf16_cstring     = const_char16_ptr;         // Null-terminated const UTF-16 string
using utf32_cstring     = const_char32_ptr;         // Null-terminated const UTF-32 string
using wide_cstring      = const_wchar_ptr;          // Null-terminated const wide string

// =================================================================================
// Define constants for minimum and maximum values of fixed-width integer types
// =================================================================================

struct Constants {
    // Minimum and maximum values for fixed-width integer types
    // These constants are defined as constexpr to allow for compile-time evaluation and optimization.

    static constexpr int8   i8min       = (-127 - 1);
    static constexpr int16  i16min      = (-32767 - 1);
    static constexpr int32  i32min      = (-2147483647 - 1);
    static constexpr int64  i64min      = (-9223372036854775807LL - 1);
    static constexpr int8   i8max       = 127;
    static constexpr int16  i16max      = 32767;
    static constexpr int32  i32max      = 2147483647;
    static constexpr int64  i64max      = 9223372036854775807LL;
    static constexpr uint8  u8max       = static_cast<uint8>(~static_cast<uint8>(0));
    static constexpr uint16 u16max      = static_cast<uint16>(~static_cast<uint16>(0));
    static constexpr uint32 u32max      = ~static_cast<uint32>(0);
    static constexpr uint64 u64max      = ~static_cast<uint64>(0ULL);

#if NEX_BUILD_ENV_IS_64_BIT
    static constexpr uint64 sizemax     = ~static_cast<uintptr>(0);
#else  // Non-64-bit environment, assume 32-bit
    static constexpr uint32 sizemax     = 0xffffffffui32;
#endif

#if NEX_PLATFORM_IS_WINDOWS
    // Windows ABI defines wchar_t/wint_t as 16-bit unsigned shorts
    static constexpr uint16 wcharmin    = 0x0000;
    static constexpr uint16 wcharmax    = 0xffff;
    static constexpr uint16 wintmin     = 0x0000;
    static constexpr uint16 wintmax     = 0xffff;
#else
    // Unix/Linux/macOS/Android ABIs define wchar_t/wint_t as 32-bit
    static constexpr uint32 wcharmin    = 0x00000000;
    static constexpr uint32 wcharmax    = 0xffffffff;
    static constexpr uint32 wintmin     = 0x00000000;
    static constexpr uint32 wintmax     = 0xffffffff;
#endif
};

// =================================================================================
// Define macros for minimum and maximum values of fixed-width integer types
// =================================================================================

/**
 * @note
 * These macros are defined to match those value in the platform SDK, ensuring compatibility 
 * with platform APIs and conventions, while also ensuring consistency across the Nex-ecosystem.
 */

#define NEX_INT8_MIN            NEX_PREPEND_NAMESPACE(Constants::i8min)
#define NEX_INT16_MIN           NEX_PREPEND_NAMESPACE(Constants::i16min)
#define NEX_INT32_MIN           NEX_PREPEND_NAMESPACE(Constants::i32min)
#define NEX_INT64_MIN           NEX_PREPEND_NAMESPACE(Constants::i64min)
#define NEX_INT8_MAX            NEX_PREPEND_NAMESPACE(Constants::i8max)
#define NEX_INT16_MAX           NEX_PREPEND_NAMESPACE(Constants::i16max)
#define NEX_INT32_MAX           NEX_PREPEND_NAMESPACE(Constants::i32max)
#define NEX_INT64_MAX           NEX_PREPEND_NAMESPACE(Constants::i64max)
#define NEX_UINT8_MAX           NEX_PREPEND_NAMESPACE(Constants::u8max)
#define NEX_UINT16_MAX          NEX_PREPEND_NAMESPACE(Constants::u16max)
#define NEX_UINT32_MAX          NEX_PREPEND_NAMESPACE(Constants::u32max)
#define NEX_UINT64_MAX          NEX_PREPEND_NAMESPACE(Constants::u64max)

#define NEX_INT_LEAST8_MIN      NEX_INT8_MIN
#define NEX_INT_LEAST16_MIN     NEX_INT16_MIN
#define NEX_INT_LEAST32_MIN     NEX_INT32_MIN
#define NEX_INT_LEAST64_MIN     NEX_INT64_MIN
#define NEX_INT_LEAST8_MAX      NEX_INT8_MAX
#define NEX_INT_LEAST16_MAX     NEX_INT16_MAX
#define NEX_INT_LEAST32_MAX     NEX_INT32_MAX
#define NEX_INT_LEAST64_MAX     NEX_INT64_MAX
#define NEX_UINT_LEAST8_MAX     NEX_UINT8_MAX
#define NEX_UINT_LEAST16_MAX    NEX_UINT16_MAX
#define NEX_UINT_LEAST32_MAX    NEX_UINT32_MAX
#define NEX_UINT_LEAST64_MAX    NEX_UINT64_MAX

#define NEX_INT_FAST8_MIN       NEX_INT8_MIN
#define NEX_INT_FAST16_MIN      NEX_INT16_MIN
#define NEX_INT_FAST32_MIN      NEX_INT32_MIN
#define NEX_INT_FAST64_MIN      NEX_INT64_MIN
#define NEX_INT_FAST8_MAX       NEX_INT8_MAX
#define NEX_INT_FAST16_MAX      NEX_INT16_MAX
#define NEX_INT_FAST32_MAX      NEX_INT32_MAX
#define NEX_INT_FAST64_MAX      NEX_INT64_MAX
#define NEX_UINT_FAST8_MAX      NEX_UINT8_MAX
#define NEX_UINT_FAST16_MAX     NEX_UINT16_MAX
#define NEX_UINT_FAST32_MAX     NEX_UINT32_MAX
#define NEX_UINT_FAST64_MAX     NEX_UINT64_MAX

#if NEX_BUILD_ENV_IS_64_BIT
    #define NEX_INTPTR_MIN      NEX_INT64_MIN
    #define NEX_INTPTR_MAX      NEX_INT64_MAX
    #define NEX_UINTPTR_MAX     NEX_UINT64_MAX
#else  // Non-64-bit environment, assume 32-bit
    #define NEX_INTPTR_MIN      NEX_INT32_MIN
    #define NEX_INTPTR_MAX      NEX_INT32_MAX
    #define NEX_UINTPTR_MAX     NEX_UINT32_MAX
#endif

#define NEX_INTMAX_MIN          NEX_INT64_MIN
#define NEX_INTMAX_MAX          NEX_INT64_MAX
#define NEX_UINTMAX_MAX         NEX_UINT64_MAX

#define NEX_PTRDIFF_MIN         NEX_INTPTR_MIN
#define NEX_PTRDIFF_MAX         NEX_INTPTR_MAX

#define NEX_SIZE_MAX            NEX_PREPEND_NAMESPACE(Constants::sizemax)

#define NEX_SIG_ATOMIC_MIN      NEX_PREPEND_NAMESPACE(Constants::i32min)
#define NEX_SIG_ATOMIC_MAX      NEX_PREPEND_NAMESPACE(Constants::i32max)

#define NEX_WCHAR_MIN           NEX_PREPEND_NAMESPACE(Constants::wcharmin)
#define NEX_WCHAR_MAX           NEX_PREPEND_NAMESPACE(Constants::wcharmax)

#define NEX_WINT_MIN            NEX_PREPEND_NAMESPACE(Constants::wintmin)
#define NEX_WINT_MAX            NEX_PREPEND_NAMESPACE(Constants::wintmax)

// =================================================================================
// Define macros for integer constant expressions
// =================================================================================

#define NEX_INT8_C(x)           (x)
#define NEX_INT16_C(x)          (x)
#define NEX_INT32_C(x)          (x)
#define NEX_INT64_C(x)          (x ## LL)

#define NEX_UINT8_C(x)          (x)
#define NEX_UINT16_C(x)         (x)
#define NEX_UINT32_C(x)         (x ## U)
#define NEX_UINT64_C(x)         (x ## ULL)

#define NEX_INTMAX_C(x)         NEX_INT64_C(x)
#define NEX_UINTMAX_C(x)        NEX_UINT64_C(x)

NEX_NAMESPACE_END
