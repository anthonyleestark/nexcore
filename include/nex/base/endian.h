/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  endian.h
 * @brief Provides byte-order detection and endianness utilities.
 *
 * @details
 * This header determines the native byte order of the target system and provides the Endian enumeration
 * for representing byte order.
 * It also provides compile-time utilities for querying whether the native byte order is little-endian
 * or big-endian.
 * Endianness detection uses compiler-provided information and, where appropriate, architecture-specific
 * information as a fallback.
 *
 * @note
 * Compilation fails if the target byte order cannot be determined reliably.
 *
 * @see
 * https://en.cppreference.com/cpp/types/endian for more information on the C++ standard endianness enumeration.
 */

#include "nex/base/namespace.h"
#include "nex/base/compiler.h"
#include "nex/base/architecture.h"
#include "nex/base/attributes.h"
#include "nex/base/visibility.h"

/**
 * @section compiler_endianness Compiler Endianness Detection
 * @brief Endianness detection using compiler predefined macros.
 * 
 * @details
 * The following preprocessor conditionals detect the endianness based on compiler-defined macros.
 * This detection is primarily relevant for GCC and Clang compilers.
 *  - `NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO` indicates the compiler detects little-endian.
 *  - `NEX_COMPILER_HAS_BIG_ENDIAN_MACRO` indicates the compiler detects big-endian.
 *  - If neither macro is defined, the compiler's endianness is considered unknown.
 */

#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
    #if !defined(__BYTE_ORDER__)
        #error Compiler does not define __BYTE_ORDER__ macro for endian detection.
    #endif
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 1
        #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 0
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 0
        #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 1
    #else
        #error Unknown byte order detected by the compiler.
    #endif
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
    #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 1
    #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 0
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
    #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 0
    #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 1
#else
    #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 0
    #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 0
#endif  // ^^Compiler endianness detection

/**
 * @section arch_endianness CPU Architecture Endianness Detection
 * @brief Endianness detection for the target CPU architecture.
 * 
 * @details
 * The following preprocessor conditionals detect the endianness of the target CPU architecture:
 *  - `NEX_ARCH_CPU_LITTLE_ENDIAN` indicates a little-endian CPU architecture.
 *  - `NEX_ARCH_CPU_BIG_ENDIAN` indicates a big-endian CPU architecture.
 *  - If neither macro is defined, the endianness is considered unknown.
 */

#if defined(NEX_ARCH_CPU_LITTLE_ENDIAN) && NEX_ARCH_CPU_LITTLE_ENDIAN
    #define NEX_ARCH_CPU_IS_LITTLE_ENDIAN 1
    #define NEX_ARCH_CPU_IS_BIG_ENDIAN 0
#elif defined(NEX_ARCH_CPU_BIG_ENDIAN) && NEX_ARCH_CPU_BIG_ENDIAN
    #define NEX_ARCH_CPU_IS_BIG_ENDIAN 1
    #define NEX_ARCH_CPU_IS_LITTLE_ENDIAN 0
#else
    #define NEX_ARCH_CPU_IS_LITTLE_ENDIAN 0
    #define NEX_ARCH_CPU_IS_BIG_ENDIAN 0
#endif  // ^^CPU architecture endianness detection

/**
 * @section endianess_detection Endianness Detection
 * @brief Detects the endianness of the target system based on compiler-provided macros and CPU architecture.
 * 
 * @note
 * This section combines the results of compiler and CPU architecture endianness detection
 * to determine the overall endianness of the target system:
 *  - If the compiler provides explicit endianness macros, they take precedence.
 *  - Otherwise, the CPU architecture endianness is used as a fallback.
 *  - If neither can be determined or if there is a conflict between compiler and CPU architecture macros,
 *    a compilation error will be raised.
 */

#if NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO
    #define NEX_LITTLE_ENDIAN
#elif NEX_COMPILER_HAS_BIG_ENDIAN_MACRO
    #define NEX_BIG_ENDIAN
#elif NEX_ARCH_CPU_IS_LITTLE_ENDIAN
    #define NEX_LITTLE_ENDIAN
#elif NEX_ARCH_CPU_IS_BIG_ENDIAN
    #define NEX_BIG_ENDIAN
#else
    #error Unable to determine the endianness of the target system.
#endif  // ^^Endianness detection

// Check for conflicts between compiler and CPU architecture endianness macros
#if (NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO && NEX_ARCH_CPU_IS_BIG_ENDIAN) || \
    (NEX_COMPILER_HAS_BIG_ENDIAN_MACRO && NEX_ARCH_CPU_IS_LITTLE_ENDIAN)
    #error Conflict between compiler and CPU architecture endianness macros.
#endif  // Endianness conflict check

NEX_NAMESPACE_BEGIN

/**
 * @enum  Endian
 * @brief Enumeration representing the endianness of the system.
 * 
 * @details
 * This enum class defines three possible values for endianness:
 *  - `Little`: Represents little-endian byte order.
 *  - `Big`: Represents big-endian byte order.
 *  - `Native`: Represents the native byte order of the system, determined at compile time.
 * 
 * @note
 * The enum values are only symbolic and implementation-defined;
 * use them strictly for semantic comparisons, never cast to raw integers.
 */
enum class Endian {
    Little = 0xDEAD,
    Big    = 0xFACE,
#if defined(NEX_LITTLE_ENDIAN)
    Native = Little
#elif defined(NEX_BIG_ENDIAN)
    Native = Big
#else
    Native = 0xCAFE
#endif
};

// ======================================================================================
// Endianness detection utility functions
// ======================================================================================

/**
 * @brief Checks if the system is little-endian.
 * @return `true` if the system is little-endian, `false` otherwise.
 */
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE
constexpr bool isLittleEndian() noexcept {
    return Endian::Native == Endian::Little;
}

/**
 * @brief Checks if the system is big-endian.
 * @return `true` if the system is big-endian, `false` otherwise.
 */
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE
constexpr bool isBigEndian() noexcept {
    return Endian::Native == Endian::Big;
}

NEX_NAMESPACE_END
