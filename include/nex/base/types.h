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
 * This header defines fundamental type aliases, including fixed-width integers, pointer-sized 
 * integers, size types, floating-point types, character types, boolean storage types, pointer types, 
 * and address types. These types are designed to provide consistent and descriptive type names 
 * across the Nex-ecosystem, while also adhering to C++20 standards and best practices for type safety 
 * and clarity, also providing Rust-style short aliases for convenience and readability.
 */

#if !defined(NEX_BASE_TYPES_NO_STD)
    #include <cstddef>
    #include <cstdint>
    #include <limits>
    #include <cwchar>
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

#include "nex/base/build.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Include standard fixed-width integer types
// =================================================================================

#if !defined(NEX_BASE_TYPES_NO_STD)
    using int8      = NEX_STD int8_t;       // 8-bit signed integer
    using uint8     = NEX_STD uint8_t;      // 8-bit unsigned integer
    using int16     = NEX_STD int16_t;      // 16-bit signed integer
    using uint16    = NEX_STD uint16_t;     // 16-bit unsigned integer
    using int32     = NEX_STD int32_t;      // 32-bit signed integer
    using uint32    = NEX_STD uint32_t;     // 32-bit unsigned integer
    using int64     = NEX_STD int64_t;      // 64-bit signed integer
    using uint64    = NEX_STD uint64_t;     // 64-bit unsigned integer
#else
    using int8      = signed char;          // 8-bit signed integer
    using uint8     = unsigned char;        // 8-bit unsigned integer
    using int16     = short;                // 16-bit signed integer
    using uint16    = unsigned short;       // 16-bit unsigned integer
    using int32     = int;                  // 32-bit signed integer
    using uint32    = unsigned int;         // 32-bit unsigned integer
    using int64     = long long;            // 64-bit signed integer
    using uint64    = unsigned long long;   // 64-bit unsigned integer
#endif  // defined(NEX_BASE_TYPES_NO_STD)

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

#if !defined(NEX_BASE_TYPES_NO_STD)
    using intptr      = NEX_STD intptr_t;   // Pointer-sized signed integer
    using uintptr     = NEX_STD uintptr_t;  // Pointer-sized unsigned integer
#else
    #if NEX_BUILD_ENV_IS_64_BIT
        using intptr  = __int64;            // Pointer-sized signed integer (64-bit)
        using uintptr = unsigned __int64;   // Pointer-sized unsigned integer (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using intptr  = int;                // Pointer-sized signed integer (32-bit)
        using uintptr = unsigned int;       // Pointer-sized unsigned integer (32-bit)
    #endif
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

// =================================================================================
// Short aliases for standard pointer-sized integer types (Rust-style)
// =================================================================================

using iptr  = intptr;                       // Pointer-sized signed integer (intptr)
using uptr  = uintptr;                      // Pointer-sized unsigned integer (uintptr)

// =================================================================================
// Include standard maximum-width integer types
// =================================================================================

#if !defined(NEX_BASE_TYPES_NO_STD)
    using intmax    = NEX_STD intmax_t;     // Maximum-width signed integer
    using uintmax   = NEX_STD uintmax_t;    // Maximum-width unsigned integer
#else
    using intmax    = long long;            // Maximum-width signed integer
    using uintmax   = unsigned long long;   // Maximum-width unsigned integer
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

// =================================================================================
// Short aliases for standard maximum-width integer types (Rust-style)
// =================================================================================

using imax  = intmax;                       // Maximum-width signed integer (intmax)
using umax  = uintmax;                      // Maximum-width unsigned integer (uintmax)

// =================================================================================
// Include standard minimum-width integer types
// =================================================================================

#if !defined(NEX_BASE_TYPES_NO_STD)
    using int_least8    = NEX_STD int_least8_t;     // Minimum 8-bit signed integer
    using uint_least8   = NEX_STD uint_least8_t;    // Minimum 8-bit unsigned integer
    using int_least16   = NEX_STD int_least16_t;    // Minimum 16-bit signed integer
    using uint_least16  = NEX_STD uint_least16_t;   // Minimum 16-bit unsigned integer
    using int_least32   = NEX_STD int_least32_t;    // Minimum 32-bit signed integer
    using uint_least32  = NEX_STD uint_least32_t;   // Minimum 32-bit unsigned integer
    using int_least64   = NEX_STD int_least64_t;    // Minimum 64-bit signed integer
    using uint_least64  = NEX_STD uint_least64_t;   // Minimum 64-bit unsigned integer
#else
    using int_least8    = signed char;              // Minimum 8-bit signed integer
    using uint_least8   = unsigned char;            // Minimum 8-bit unsigned integer
    using int_least16   = short;                    // Minimum 16-bit signed integer
    using uint_least16  = unsigned short;           // Minimum 16-bit unsigned integer
    using int_least32   = int;                      // Minimum 32-bit signed integer
    using uint_least32  = unsigned int;             // Minimum 32-bit unsigned integer
    using int_least64   = long long;                // Minimum 64-bit signed integer
    using uint_least64  = unsigned long long;       // Minimum 64-bit unsigned integer
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

#if NEX_HAS_BUILTIN_INT128
    using int_least128   = int128;                  // Minimum 128-bit signed integer
    using uint_least128  = uint128;                 // Minimum 128-bit unsigned integer
#else
    using int_least128   = int_least64;             // No support for 128-bit signed integer, fallback to 64-bit
    using uint_least128  = uint_least64;            // No support for 128-bit unsigned integer, fallback to 64-bit
#endif

// =================================================================================
// Include standard fastest minimum-width integer types
// =================================================================================

#if !defined(NEX_BASE_TYPES_NO_STD)
    using int_fast8     = NEX_STD int_fast8_t;      // Fastest minimum 8-bit signed integer
    using uint_fast8    = NEX_STD uint_fast8_t;     // Fastest minimum 8-bit unsigned integer
    using int_fast16    = NEX_STD int_fast16_t;     // Fastest minimum 16-bit signed integer
    using uint_fast16   = NEX_STD uint_fast16_t;    // Fastest minimum 16-bit unsigned integer
    using int_fast32    = NEX_STD int_fast32_t;     // Fastest minimum 32-bit signed integer
    using uint_fast32   = NEX_STD uint_fast32_t;    // Fastest minimum 32-bit unsigned integer
    using int_fast64    = NEX_STD int_fast64_t;     // Fastest minimum 64-bit signed integer
    using uint_fast64   = NEX_STD uint_fast64_t;    // Fastest minimum 64-bit unsigned integer
#else
    using int_fast8     = signed char;              // Fastest minimum 8-bit signed integer
    using uint_fast8    = unsigned char;            // Fastest minimum 8-bit unsigned integer
    using int_fast16    = int;                      // Fastest minimum 16-bit signed integer
    using uint_fast16   = unsigned int;             // Fastest minimum 16-bit unsigned integer
    using int_fast32    = int;                      // Fastest minimum 32-bit signed integer
    using uint_fast32   = unsigned int;             // Fastest minimum 32-bit unsigned integer
    using int_fast64    = long long;                // Fastest minimum 64-bit signed integer
    using uint_fast64   = unsigned long long;       // Fastest minimum 64-bit unsigned integer
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

#if NEX_HAS_BUILTIN_INT128
    using int_fast128    = int128;                  // Fastest minimum 128-bit signed integer
    using uint_fast128   = uint128;                 // Fastest minimum 128-bit unsigned integer
#else
    using int_fast128    = int_fast64;              // No support for 128-bit signed integer, fallback to 64-bit
    using uint_fast128   = uint_fast64;             // No support for 128-bit unsigned integer, fallback to 64-bit
#endif

// =================================================================================
// Include standard size types
// =================================================================================

#if !defined(NEX_BASE_TYPES_NO_STD)
    using sizetype  = NEX_STD size_t;           // Unsigned integer type used for sizes & array indexing
    using ptrdiff   = NEX_STD ptrdiff_t;        // Signed integer type used for pointer arithmetic & array indexing
    using max_align = NEX_STD max_align_t;      // Type with alignment at least as strict as any scalar type
#else
    #if NEX_BUILD_ENV_IS_64_BIT
        using sizetype  = unsigned __int64;     // Unsigned integer type used for sizes & array indexing (64-bit)
        using ptrdiff   = __int64;              // Signed integer type used for pointer arithmetic & array indexing (64-bit)
    #else  // Non-64-bit environment, assume 32-bit
        using sizetype  = unsigned int;         // Unsigned integer type used for sizes & array indexing (32-bit)
        using ptrdiff   = int;                  // Signed integer type used for pointer arithmetic & array indexing (32-bit)
    #endif
    using max_align = double;                   // Type with alignment at least as strict as any scalar type
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

using usize         = sizetype;                 // Unsigned integer type used for sizes & array indexing
using isize         = ptrdiff;                  // Signed integer type used for pointer arithmetic & array indexing

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

#if !defined(NEX_BASE_TYPES_NO_STD)
    using wint  = NEX_STD wint_t;           // Wide character type for I/O functions
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

#if !defined(NEX_BASE_TYPES_NO_STD)
    using null_ptr  = NEX_STD nullptr_t;            // Null pointer type
    using byte      = NEX_STD byte;                 // Single-byte object-representation type
#else
    using null_ptr  = decltype(nullptr);            // Null pointer type
    enum class byte : unsigned char {};             // Single-byte object-representation type

    /**
     * @note
     * Internal implementation of `EnableIf` and related utilities for SFINAE and type traits.
     * Since we are not including the full `<type_traits>` header, we need to provide our own minimal 
     * implementation of `EnableIf` and related utilities to support the type traits used in this header.
     */

    // EnableIf implementation for SFINAE
    template <bool __BoolCond, class __Type = void>
    struct __EnableIf {};               // no member "type" when !BoolCond

    // Specialization of EnableIf for when the condition is true
    template <class __Type>
    struct __EnableIf<true, __Type> {   // type is __Type for __BoolCond
        using type = __Type;
    };

    template <bool __BoolCond, class __Type = void>
    using __EnableIfT = typename __EnableIf<__BoolCond, __Type>::type;

    // RemoveCv implementation to remove top-level const and volatile qualifiers
    template <class __Type>
    struct __RemoveCv {
        using type = __Type;

        // apply cv-qualifiers from the class template argument to __Fn<__Type>
        template <template <class> class __Fn>
        using __Apply = __Fn<__Type>;
    };

    template <class __Type>
    struct __RemoveCv<const __Type> {
        using type = __Type;
        template <template <class> class __Fn>
        using __Apply = const __Fn<__Type>;
    };

    template <class __Type>
    struct __RemoveCv<volatile __Type> {
        using type = __Type;
        template <template <class> class __Fn>
        using __Apply = volatile __Fn<__Type>;
    };

    template <class __Type>
    struct __RemoveCv<const volatile __Type> {
        using type = __Type;
        template <template <class> class __Fn>
        using __Apply = const volatile __Fn<__Type>;
    };

    template <class __Type>
    using __RemoveCvT = typename __RemoveCv<__Type>::type;

    #if NEX_COMPILER_IS_CLANG
        // Clang provides a builtin type trait for is_same, 
        // which is more efficient than our implementation, so use it when available
        template <class __Type1, class __Type2>
        constexpr bool __IsSameV = __is_same(__Type1, __Type2);
    #else
        template <class, class>
        constexpr bool __IsSameV = false;   // determine whether arguments are the same type
        template <class __Type>
        constexpr bool __IsSameV<__Type, __Type> = true;
    #endif

    template <class __Type, class... __Types>
    constexpr bool __IsAnyOfV =             // true if and only if __Type is in __Types
    #if NEX_HAS_CXX17
        (__IsSameV<__Type, __Types> || ...);
    #else // C++14 or earlier
        // disjunction_v is defined in <xtr1common>, 
        // but we do NOT need it here since we only supports C++20 or later
        disjunction_v<is_same<__Type, __Types>...>;
    #endif // NEX_HAS_CXX17

    template <class __Type>
    constexpr bool __IsIntegralV = __IsAnyOfV<
        __RemoveCvT<__Type>, bool, 
        char, signed char, unsigned char, wchar_t,
        #if defined(__cpp_char8_t)
            char8_t,
        #endif // defined(__cpp_char8_t)
        char16_t, char32_t, 
        short, unsigned short, 
        int, unsigned int, 
        long, unsigned long, 
        long long, unsigned long long
    >;

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
    template <class IntType, __EnableIfT<__IsIntegralV<IntType>, int> = 0>
    NEX_NODISCARD constexpr byte operator<<(const byte arg, const IntType shift) noexcept {
        return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(arg) << shift));
    }

    // Bitwise right shift operator for 'byte' type
    template <class IntType, __EnableIfT<__IsIntegralV<IntType>, int> = 0>
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
    template <class IntType, __EnableIfT<__IsIntegralV<IntType>, int> = 0>
    constexpr byte& operator<<=(byte& arg, const IntType shift) noexcept {
        return arg = arg << shift;
    }

    // Bitwise right shift compound assignment operator for 'byte' type
    template <class IntType, __EnableIfT<__IsIntegralV<IntType>, int> = 0>
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
    template <class IntType, __EnableIfT<__IsIntegralV<IntType>, int> = 0>
    NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr IntType toInteger(const byte arg) noexcept {
        return static_cast<IntType>(arg);
    }
#endif  // !defined(NEX_BASE_TYPES_NO_STD)

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

#if !defined(NEX_BASE_TYPES_NO_STD)
    static constexpr int8   i8min         = NEX_STD numeric_limits<int8>::min();
    static constexpr int16  i16min        = NEX_STD numeric_limits<int16>::min();
    static constexpr int32  i32min        = NEX_STD numeric_limits<int32>::min();
    static constexpr int64  i64min        = NEX_STD numeric_limits<int64>::min();
    static constexpr int8   i8max         = NEX_STD numeric_limits<int8>::max();
    static constexpr int16  i16max        = NEX_STD numeric_limits<int16>::max();
    static constexpr int32  i32max        = NEX_STD numeric_limits<int32>::max();
    static constexpr int64  i64max        = NEX_STD numeric_limits<int64>::max();
    static constexpr uint8  u8max         = NEX_STD numeric_limits<uint8>::max();
    static constexpr uint16 u16max        = NEX_STD numeric_limits<uint16>::max();
    static constexpr uint32 u32max        = NEX_STD numeric_limits<uint32>::max();
    static constexpr uint64 u64max        = NEX_STD numeric_limits<uint64>::max();

    static constexpr uint64 sizemax       = NEX_STD numeric_limits<sizetype>::max();
    static constexpr uint16 wcharmin      = NEX_STD numeric_limits<wchar>::min();
    static constexpr uint16 wcharmax      = NEX_STD numeric_limits<wchar>::max();
    static constexpr uint16 wintmin       = NEX_STD numeric_limits<wint>::min();
    static constexpr uint16 wintmax       = NEX_STD numeric_limits<wint>::max();
#else
    static constexpr int8   i8min         = (-127i8 - 1);
    static constexpr int16  i16min        = (-32767i16 - 1);
    static constexpr int32  i32min        = (-2147483647i32 - 1);
    static constexpr int64  i64min        = (-9223372036854775807i64 - 1);
    static constexpr int8   i8max         = 127i8;
    static constexpr int16  i16max        = 32767i16;
    static constexpr int32  i32max        = 2147483647i32;
    static constexpr int64  i64max        = 9223372036854775807i64;
    static constexpr uint8  u8max         = 0xffui8;
    static constexpr uint16 u16max        = 0xffffui16;
    static constexpr uint32 u32max        = 0xffffffffui32;
    static constexpr uint64 u64max        = 0xffffffffffffffffui64;

    #if NEX_BUILD_ENV_IS_64_BIT
        static constexpr uint64 sizemax   = 0xffffffffffffffffui64;
    #else  // Non-64-bit environment, assume 32-bit
        static constexpr uint32 sizemax   = 0xffffffffui32;
    #endif
    static constexpr uint16 wcharmin      = 0x0000;
    static constexpr uint16 wcharmax      = 0xffff;
    static constexpr uint16 wintmin       = 0x0000;
    static constexpr uint16 wintmax       = 0xffff;
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
