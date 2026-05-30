/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  limits.h
 * @brief Custom, zero-dependency compile-time numerical limits and type traits.
 * 
 * @details 
 * Portable alternative to platform-specific system headers. Designed for freestanding, bare-metal, 
 * and multi-platform environments.Guaranteed zero runtime overhead via `constexpr` evaluations.
 */

#include "nex/base/platform.h"
#include "nex/base/compiler.h"
#include "nex/base/build.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

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