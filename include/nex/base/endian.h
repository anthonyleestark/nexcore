/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    endian.h
 * @brief   Endianness detection and utilities.
 * 
 * @details
 * This header provides utilities for detecting the endianness of the target architecture at compile time.
 * It defines an enumeration `Endian` that represents the endianness of the system, with values for little-endian, 
 * big-endian, and native endianness.
 * The endianness detection relies on predefined macros that are set by the compiler or the build environment.
 * If the endianness cannot be determined, a compilation error will be raised.
 */

#include "nex/base/namespace.h"
#include "nex/base/compiler.h"
#include "nex/base/architecture.h"
#include "nex/base/attributes.h"

/**
 * @note
 * The endianness detection relies on predefined macros that are set by the compiler or the build environment. 
 * If the endianness cannot be determined, a compilation error will be raised.
 */

#if NEX_ARCH_CPU_IS_LITTLE_ENDIAN || NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO
    #define NEX_LITTLE_ENDIAN   // Little Endian
#elif NEX_ARCH_CPU_IS_BIG_ENDIAN || NEX_COMPILER_HAS_BIG_ENDIAN_MACRO
    #define NEX_BIG_ENDIAN      // Big Endian
#else
    #error Unable to detect endianness of the target architecture.
    #error Please add support for your endianness in include/nex/base/architecture.h and include/nex/base/compiler.h
#endif  // ^^ Endianness detection

NEX_NAMESPACE_BEGIN

/**
 * @enum  Endian
 * @brief Enumeration representing the endianness of the system.
 * 
 * @details
 * This enum class defines three possible values for endianness:
 * - `Little`: Represents little-endian byte order.
 * - `Big`: Represents big-endian byte order.
 * - `Native`: Represents the native byte order of the system, determined at compile time.
 * 
 * @note
 * The enum values are only symbolic and implementation-defined;
 * use them strictly for semantic comparisons, never cast to raw integers.
 * 
 * @see libc++'s `std::endian` for a similar implementation in the C++ standard library.
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
