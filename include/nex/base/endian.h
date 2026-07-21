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
 * This header defines the `Endian` enum class, which represents the endianness of the system. It provides three possible values:
 * - `Little`: Represents little-endian byte order.
 * - `Big`: Represents big-endian byte order.
 * - `Native`: Represents the native byte order of the system, determined at compile time.
 * 
 * The endianness is determined based on predefined macros that indicate the target architecture's byte order.
 */

#include "nex/base/namespace.h"
#include "nex/base/compiler.h"
#include "nex/base/architecture.h"

/**
 * @note
 * The endianness detection relies on predefined macros that are set by the compiler or the build environment. 
 * If the endianness cannot be determined, a compilation error will be raised.
 */

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) || \
    defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || \
    defined(_M_IX86) || defined(_M_X64)
    #define NEX_LITTLE_ENDIAN   // Little Endian
#elif (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || \
      defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || \
      defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
      defined(_M_PPC) || defined(_M_PPCBE)
    #define NEX_BIG_ENDIAN      // Big Endian
#else
    #error Unable to detect endianness of the target architecture.
#endif

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

NEX_NAMESPACE_END
