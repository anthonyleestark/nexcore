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

#include <cstddef>
#include <cstdint>

#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Include standard fixed-width integer types
// ============================================================================

using int8      = NEX_STD int8_t;           // 8-bit signed integer
using uint8     = NEX_STD uint8_t;          // 8-bit unsigned integer
using int16     = NEX_STD int16_t;          // 16-bit signed integer
using uint16    = NEX_STD uint16_t;         // 16-bit unsigned integer
using int32     = NEX_STD int32_t;          // 32-bit signed integer
using uint32    = NEX_STD uint32_t;         // 32-bit unsigned integer
using int64     = NEX_STD int64_t;          // 64-bit signed integer
using uint64    = NEX_STD uint64_t;         // 64-bit unsigned integer

// ============================================================================
// Short aliases for standard fixed-width integer types (Rust-style)
// ============================================================================

using i8        = int8;                     // 8-bit signed integer (int8)
using u8        = uint8;                    // 8-bit unsigned integer (uint8)
using i16       = int16;                    // 16-bit signed integer (int16)
using u16       = uint16;                   // 16-bit unsigned integer (uint16)
using i32       = int32;                    // 32-bit signed integer (int32)
using u32       = uint32;                   // 32-bit unsigned integer (uint32)
using i64       = int64;                    // 64-bit signed integer (int64)
using u64       = uint64;                   // 64-bit unsigned integer (uint64)

// ============================================================================
// Include standard pointer-sized integer types
// ============================================================================

using intptr    = NEX_STD intptr_t;         // Pointer-sized signed integer
using uintptr   = NEX_STD uintptr_t;        // Pointer-sized unsigned integer

// ============================================================================
// Short aliases for standard pointer-sized integer types (Rust-style)
// ============================================================================

using iptr      = intptr;                   // Pointer-sized signed integer (intptr)
using uptr      = uintptr;                  // Pointer-sized unsigned integer (uintptr)

// ============================================================================
// Include standard maximum-width integer types
// ============================================================================

using intmax    = NEX_STD intmax_t;         // Maximum-width signed integer
using uintmax   = NEX_STD uintmax_t;        // Maximum-width unsigned integer

// ============================================================================
// Short aliases for standard maximum-width integer types (Rust-style)
// ============================================================================

using imax      = intmax;                   // Maximum-width signed integer (intmax)
using umax      = uintmax;                  // Maximum-width unsigned integer (uintmax)

// ============================================================================
// Include standard minimum-width integer types
// ============================================================================

using int_least8        = NEX_STD int_least8_t;         // Minimum 8-bit signed integer
using uint_least8       = NEX_STD uint_least8_t;        // Minimum 8-bit unsigned integer
using int_least16       = NEX_STD int_least16_t;        // Minimum 16-bit signed integer
using uint_least16      = NEX_STD uint_least16_t;       // Minimum 16-bit unsigned integer
using int_least32       = NEX_STD int_least32_t;        // Minimum 32-bit signed integer
using uint_least32      = NEX_STD uint_least32_t;       // Minimum 32-bit unsigned integer
using int_least64       = NEX_STD int_least64_t;        // Minimum 64-bit signed integer
using uint_least64      = NEX_STD uint_least64_t;       // Minimum 64-bit unsigned integer

// ============================================================================
// Include standard fastest minimum-width integer types
// ============================================================================

using int_fast8         = NEX_STD int_fast8_t;          // Fastest minimum 8-bit signed integer
using uint_fast8        = NEX_STD uint_fast8_t;         // Fastest minimum 8-bit unsigned integer
using int_fast16        = NEX_STD int_fast16_t;         // Fastest minimum 16-bit signed integer
using uint_fast16       = NEX_STD uint_fast16_t;        // Fastest minimum 16-bit unsigned integer
using int_fast32        = NEX_STD int_fast32_t;         // Fastest minimum 32-bit signed integer
using uint_fast32       = NEX_STD uint_fast32_t;        // Fastest minimum 32-bit unsigned integer
using int_fast64        = NEX_STD int_fast64_t;         // Fastest minimum 64-bit signed integer
using uint_fast64       = NEX_STD uint_fast64_t;        // Fastest minimum 64-bit unsigned integer

// ============================================================================
// Include standard size types
// ============================================================================

using usize         = NEX_STD size_t;       // Unsigned integer type used for sizes and array indexing
using isize         = NEX_STD ptrdiff_t;    // Signed integer type used for pointer arithmetic and array indexing
using max_align     = NEX_STD max_align_t;  // Type with alignment at least as strict as any scalar type

// ============================================================================
// Short aliases for standard size types (Rust-style)
// ============================================================================

using size          = usize;        // Unsigned integer type used for sizes and array indexing (usize)
using ssize         = isize;        // Signed integer type used for pointer arithmetic and array indexing (isize)

// ============================================================================
// Include standard floating-point types
// ============================================================================

using float32       = float;                // 32-bit floating point
using float64       = double;               // 64-bit floating point
using ldouble       = long double;          // Extended precision floating point
using floatmax      = long double;          // Widest standard floating-point type
#if NEX_HAS_BUILTIN_FLOAT128
    using float128  = __float128;           // 128-bit floating point
#else
    using float128  = long double;          // Widest built-in floating-point fallback
#endif

// ============================================================================
// Short aliases for standard floating-point types (Rust-style)
// ============================================================================

using f32       = float32;                  // 32-bit floating point (float32)
using f64       = float64;                  // 64-bit floating point (float64)
using f128      = float128;                 // 128-bit floating point (float128)

// ============================================================================
// Include standard character types (encoding-aware; C++20)
// ============================================================================

using char8     = char8_t;                  // 8-bit character type (UTF-8)
using char16    = char16_t;                 // 16-bit character type (UTF-16)
using char32    = char32_t;                 // 32-bit character type (UTF-32)
using wchar     = wchar_t;                  // Wide character type
using schar     = signed char;              // Signed narrow character storage type
using uchar     = unsigned char;            // Unsigned narrow character storage type
using codepoint = char32;                   // Unicode code point storage type

// ============================================================================
// Short aliases for standard character types (Rust-style)
// ============================================================================

using c8        = char8;                    // 8-bit character type (UTF-8)
using c16       = char16;                   // 16-bit character type (UTF-16)
using c32       = char32;                   // 32-bit character type (UTF-32)

// ============================================================================
// Include boolean types (using uint8 and uint32 for boolean storage)
// ============================================================================

using boolean   = bool;                     // Boolean value (true or false)
using bool8     = uint8;                    // 8-bit boolean storage value (0 = false, non-zero = true)
using bool32    = uint32;                   // 32-bit boolean storage value (0 = false, non-zero = true)

// ============================================================================
// Short aliases for boolean types (Rust-style)
// ============================================================================

using b8    = uint8;                        // 8-bit boolean storage value (0 = false, non-zero = true)
using b32   = uint32;                       // 32-bit boolean storage value (0 = false, non-zero = true)

// ============================================================================
// Include pointer and address-related types
// ============================================================================

using null_ptr              = NEX_STD nullptr_t;    // Null pointer type
using byte                  = NEX_STD byte;         // Single-byte object-representation type
using raw_byte              = uint8;                // Arithmetic byte type
using byte_ptr              = raw_byte*;            // Pointer to arithmetic byte data
using const_byte_ptr        = const raw_byte*;      // Pointer to const arithmetic byte data
using std_byte_ptr          = byte*;                // Pointer to byte data
using const_std_byte_ptr    = const byte*;          // Pointer to const byte data
using void_ptr              = void*;                // Pointer to void
using const_void_ptr        = const void*;          // Pointer to const void
using address               = uintptr;              // Memory address (pointer-sized unsigned integer)

// ============================================================================
// Include character pointer types (encoding-aware; C++20)
// ============================================================================

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

// ============================================================================
// Include C-style null-terminated string types (encoding-aware; C++20)
// ============================================================================

using cstring           = const_char_ptr;           // Null-terminated const char string
using utf8_cstring      = const_char8_ptr;          // Null-terminated const UTF-8 string
using utf16_cstring     = const_char16_ptr;         // Null-terminated const UTF-16 string
using utf32_cstring     = const_char32_ptr;         // Null-terminated const UTF-32 string
using wide_cstring      = const_wchar_ptr;          // Null-terminated const wide string

NEX_NAMESPACE_END
