/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

/**
 * @file  types.h
 * @brief Defines common types used throughout the codebase.
 * 
 * @details
 * This file defines common types used throughout the codebase, such as fixed-width integer types (e.g., int32, uint64), 
 * size types (e.g., usize, isize), floating-point types (e.g., float32, float64), character types (e.g., char8, char16), 
 * pointer types (e.g., byte_ptr, char_ptr).
 * The types defined in this file can be used throughout the codebase to improve code readability and maintainability 
 * by providing consistent type names and aliases for commonly used types.
 * The use of fixed-width integer types ensures that the code behaves consistently across different platforms and compilers.
 * The use of size types allows for better handling of memory sizes and array indexing.
 * The use of character types allows for better handling of different character encodings.
 * The use of pointer types allows for better handling of raw memory and data manipulation.
 */

NEX_NAMESPACE_BEGIN

// ================================================================================================
// Include standard integer types
// ================================================================================================

using int8 = NEX_STD int8_t;                    // 8-bit signed integer
using uint8 = NEX_STD uint8_t;                  // 8-bit unsigned integer
using int16 = NEX_STD int16_t;                  // 16-bit signed integer
using uint16 = NEX_STD uint16_t;                // 16-bit unsigned integer
using int32 = NEX_STD int32_t;                  // 32-bit signed integer
using uint32 = NEX_STD uint32_t;                // 32-bit unsigned integer
using int64 = NEX_STD int64_t;                  // 64-bit signed integer
using uint64 = NEX_STD uint64_t;                // 64-bit unsigned integer

// ================================================================================================
// Include standard pointer types
// ================================================================================================

using intptr = NEX_STD intptr_t;                // Pointer-sized signed integer
using uintptr = NEX_STD uintptr_t;              // Pointer-sized unsigned integer

// ================================================================================================
// Include standard max-width integer types
// ================================================================================================

using intmax = NEX_STD intmax_t;                // Maximum-width signed integer
using uintmax = NEX_STD uintmax_t;              // Maximum-width unsigned integer

// ================================================================================================
// Include standard least-width integer types
// ================================================================================================

using int_least8 = NEX_STD int_least8_t;        // Minimum 8-bit signed integer
using uint_least8 = NEX_STD uint_least8_t;      // Minimum 8-bit unsigned integer
using int_least16 = NEX_STD int_least16_t;      // Minimum 16-bit signed integer
using uint_least16 = NEX_STD uint_least16_t;    // Minimum 16-bit unsigned integer
using int_least32 = NEX_STD int_least32_t;      // Minimum 32-bit signed integer
using uint_least32 = NEX_STD uint_least32_t;    // Minimum 32-bit unsigned integer
using int_least64 = NEX_STD int_least64_t;      // Minimum 64-bit signed integer
using uint_least64 = NEX_STD uint_least64_t;    // Minimum 64-bit unsigned integer

// ================================================================================================
// Include standard fast-width integer types
// ================================================================================================

using int_fast8 = NEX_STD int_fast8_t;          // Fastest minimum 8-bit signed integer
using uint_fast8 = NEX_STD uint_fast8_t;        // Fastest minimum 8-bit unsigned integer
using int_fast16 = NEX_STD int_fast16_t;        // Fastest minimum 16-bit signed integer
using uint_fast16 = NEX_STD uint_fast16_t;      // Fastest minimum 16-bit unsigned integer
using int_fast32 = NEX_STD int_fast32_t;        // Fastest minimum 32-bit signed integer
using uint_fast32 = NEX_STD uint_fast32_t;      // Fastest minimum 32-bit unsigned integer
using int_fast64 = NEX_STD int_fast64_t;        // Fastest minimum 64-bit signed integer
using uint_fast64 = NEX_STD uint_fast64_t;      // Fastest minimum 64-bit unsigned integer

// ================================================================================================
// Include standard size types
// ================================================================================================

using usize = NEX_STD size_t;       // Unsigned integer type used for sizes and array indexing
using isize = NEX_STD ptrdiff_t;    // Signed integer type used for pointer arithmetic and array indexing

// ================================================================================================
// Include standard floating point types
// ================================================================================================

using float32 = float;                      // 32-bit floating point
using float64 = double;                     // 64-bit floating point
#if NEX_HAS_BUILTIN_FLOAT128
    using float128 = __float128;            // 128-bit floating point
#else
    using float128 = long double;           // 128-bit floating point
#endif

// ================================================================================================
// Include character types
// ================================================================================================

using char8 = char8_t;                      // 8-bit character type (UTF-8)
using char16 = char16_t;                    // 16-bit character type (UTF-16)
using char32 = char32_t;                    // 32-bit character type (UTF-32)

// ================================================================================================
// Include pointer types
// ================================================================================================

using null_ptr = NEX_STD nullptr_t;         // Null pointer type
using byte = NEX_STD byte;                  // Byte type (unsigned char)
using byte_ptr = uint8*;                    // Pointer to byte (unsigned char*)
using const_byte_ptr = const uint8*;        // Pointer to const byte (const unsigned char*)
using std_byte_ptr = byte*;                 // Pointer to std::byte
using const_std_byte_ptr = const byte*;     // Pointer to const std::byte
using char_ptr = char*;                     // Pointer to char
using const_char_ptr = const char*;         // Pointer to const char
using wchar_ptr = wchar_t*;                 // Pointer to wide character
using const_wchar_ptr = const wchar_t*;     // Pointer to const wide character
using void_ptr = void*;                     // Pointer to void
using const_void_ptr = const void*;         // Pointer to const void
using address = uintptr;                    // Memory address (pointer-sized unsigned integer)

NEX_NAMESPACE_END
