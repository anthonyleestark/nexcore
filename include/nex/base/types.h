/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "common/macros.h"

/**
 * @brief Defines common types used throughout the codebase, such as fixed-width integer types, size types, 
 *        floating-point types, character types, pointer types, and forward declarations of common classes.
 * 
 * @details
 * This file defines common types used throughout the codebase, such as fixed-width integer types (e.g., int32, uint64), 
 * size types (e.g., usize, isize), floating-point types (e.g., float32, float64), character types (e.g., char8, char16), 
 * pointer types (e.g., byte_ptr, char_ptr), and forward declarations of common classes (e.g., String, StringView).
 * The types defined in this file can be used throughout the codebase to improve code readability and maintainability 
 * by providing consistent type names and aliases for commonly used types.
 * The use of fixed-width integer types ensures that the code behaves consistently across different platforms and compilers.
 * The use of size types allows for better handling of memory sizes and array indexing.
 * The use of character types allows for better handling of different character encodings.
 * The use of pointer types allows for better handling of raw memory and data manipulation.
 */

NEXSUITE_NAMESPACE_BEGIN

// Include standard integer types
using int8 = NEXSUITE_STD int8_t;
using uint8 = NEXSUITE_STD uint8_t;
using int16 = NEXSUITE_STD int16_t;
using uint16 = NEXSUITE_STD uint16_t;
using int32 = NEXSUITE_STD int32_t;
using uint32 = NEXSUITE_STD uint32_t;
using int64 = NEXSUITE_STD int64_t;
using uint64 = NEXSUITE_STD uint64_t;

// Include standard pointer types
using intptr = NEXSUITE_STD intptr_t;
using uintptr = NEXSUITE_STD uintptr_t;

// Include standard max-width integer types
using intmax = NEXSUITE_STD intmax_t;
using uintmax = NEXSUITE_STD uintmax_t;

// Include standard least-width integer types
using int_least8 = NEXSUITE_STD int_least8_t;
using uint_least8 = NEXSUITE_STD uint_least8_t;
using int_least16 = NEXSUITE_STD int_least16_t;
using uint_least16 = NEXSUITE_STD uint_least16_t;
using int_least32 = NEXSUITE_STD int_least32_t;
using uint_least32 = NEXSUITE_STD uint_least32_t;
using int_least64 = NEXSUITE_STD int_least64_t;
using uint_least64 = NEXSUITE_STD uint_least64_t;

// Include standard fast-width integer types
using int_fast8 = NEXSUITE_STD int_fast8_t;
using uint_fast8 = NEXSUITE_STD uint_fast8_t;
using int_fast16 = NEXSUITE_STD int_fast16_t;
using uint_fast16 = NEXSUITE_STD uint_fast16_t;
using int_fast32 = NEXSUITE_STD int_fast32_t;
using uint_fast32 = NEXSUITE_STD uint_fast32_t;
using int_fast64 = NEXSUITE_STD int_fast64_t;
using uint_fast64 = NEXSUITE_STD uint_fast64_t;

// Include standard size types
using usize = NEXSUITE_STD size_t;
using isize = NEXSUITE_STD ptrdiff_t;

// Include standard floating point types
using float32 = float;
using float64 = double;
#if NEXSUITE_HAS_BUILTIN_FLOAT128
    using float128 = __float128;
#else
    using float128 = long double;
#endif

// Include character types
using char8 = char8_t;
using char16 = char16_t;
using char32 = char32_t;

// Include pointer types
using null_ptr = NEXSUITE_STD nullptr_t;
using byte = NEXSUITE_STD byte;
using byte_ptr = uint8*;
using const_byte_ptr = const uint8*;
using std_byte_ptr = byte*;
using const_std_byte_ptr = const byte*;
using char_ptr = char*;
using const_char_ptr = const char*;
using wchar_ptr = wchar_t*;
using const_wchar_ptr = const wchar_t*;
using void_ptr = void*;
using const_void_ptr = const void*;
using address = uintptr;  // Represents a memory address (pointer-sized unsigned integer)

// Forward declare common types
class String;
class StringView;

NEXSUITE_NAMESPACE_END
