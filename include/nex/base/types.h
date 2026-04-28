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

NEX_NAMESPACE_BEGIN

// Include standard integer types
using int8 = NEX_STD int8_t;
using uint8 = NEX_STD uint8_t;
using int16 = NEX_STD int16_t;
using uint16 = NEX_STD uint16_t;
using int32 = NEX_STD int32_t;
using uint32 = NEX_STD uint32_t;
using int64 = NEX_STD int64_t;
using uint64 = NEX_STD uint64_t;

// Include standard pointer types
using intptr = NEX_STD intptr_t;
using uintptr = NEX_STD uintptr_t;

// Include standard max-width integer types
using intmax = NEX_STD intmax_t;
using uintmax = NEX_STD uintmax_t;

// Include standard least-width integer types
using int_least8 = NEX_STD int_least8_t;
using uint_least8 = NEX_STD uint_least8_t;
using int_least16 = NEX_STD int_least16_t;
using uint_least16 = NEX_STD uint_least16_t;
using int_least32 = NEX_STD int_least32_t;
using uint_least32 = NEX_STD uint_least32_t;
using int_least64 = NEX_STD int_least64_t;
using uint_least64 = NEX_STD uint_least64_t;

// Include standard fast-width integer types
using int_fast8 = NEX_STD int_fast8_t;
using uint_fast8 = NEX_STD uint_fast8_t;
using int_fast16 = NEX_STD int_fast16_t;
using uint_fast16 = NEX_STD uint_fast16_t;
using int_fast32 = NEX_STD int_fast32_t;
using uint_fast32 = NEX_STD uint_fast32_t;
using int_fast64 = NEX_STD int_fast64_t;
using uint_fast64 = NEX_STD uint_fast64_t;

// Include standard size types
using usize = NEX_STD size_t;
using isize = NEX_STD ptrdiff_t;

// Include standard floating point types
using float32 = float;
using float64 = double;
#if NEX_HAS_BUILTIN_FLOAT128
    using float128 = __float128;
#else
    using float128 = long double;
#endif

// Include character types
using char8 = char8_t;
using char16 = char16_t;
using char32 = char32_t;

// Include pointer types
using null_ptr = NEX_STD nullptr_t;
using byte = NEX_STD byte;
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

NEX_NAMESPACE_END
