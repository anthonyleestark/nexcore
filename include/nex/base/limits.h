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
// Compile-time constants for fixed-width integer limits and character type limits
// =================================================================================

struct LimitConstants {
    /**
     * @note
     * These constants are defined to match those value in the platform SDK, ensuring compatibility
     * with platform APIs and conventions, while also ensuring consistency across the Nex-ecosystem.
     */

    static constexpr int8   i8min       = (-127_i8 - 1_i8);
    static constexpr int16  i16min      = (-32767_i16 - 1_i16);
    static constexpr int32  i32min      = (-2147483647_i32 - 1_i32);
    static constexpr int64  i64min      = (-9223372036854775807_i64 - 1_i64);
    static constexpr int8   i8max       = 127_i8;
    static constexpr int16  i16max      = 32767_i16;
    static constexpr int32  i32max      = 2147483647_i32;
    static constexpr int64  i64max      = 9223372036854775807_i64;
    static constexpr uint8  u8max       = static_cast<uint8>(~static_cast<uint8>(0));
    static constexpr uint16 u16max      = static_cast<uint16>(~static_cast<uint16>(0));
    static constexpr uint32 u32max      = ~static_cast<uint32>(0);
    static constexpr uint64 u64max      = ~static_cast<uint64>(0);

#if NEX_HAS_BUILTIN_INT128
    static constexpr int128  i128min    = (static_cast<int128>(-1) << 127_i128);
    static constexpr int128  i128max    = ~(static_cast<int128>(-1) << 127_i128);
    static constexpr uint128 u128max    = ~static_cast<uint128>(0);
#endif  // ^^NEX_HAS_BUILTIN_INT128

#if NEX_BUILD_ENV_IS_64_BIT
    static constexpr uint64 sizemax     = ~static_cast<uintptr>(0_u64);
#else  // Non-64-bit environment, assume 32-bit
    static constexpr uint32 sizemax     = 0xffffffff_u32;
#endif

#if NEX_PLATFORM_IS_WINDOWS
    // Windows ABI defines wchar_t/wint_t as 16-bit unsigned shorts
    static constexpr uint16 wcharmin    = 0x0000_u16;
    static constexpr uint16 wcharmax    = 0xffff_u16;
    static constexpr uint16 wintmin     = 0x0000_u16;
    static constexpr uint16 wintmax     = 0xffff_u16;
#else
    // Unix/Linux/macOS/Android ABIs define wchar_t/wint_t as 32-bit
    static constexpr uint32 wcharmin    = 0x00000000_u32;
    static constexpr uint32 wcharmax    = 0xffffffff_u32;
    static constexpr uint32 wintmin     = 0x00000000_u32;
    static constexpr uint32 wintmax     = 0xffffffff_u32;
#endif
};

// =================================================================================
// Macro definitions for fixed-width integer limits (C-style)
// =================================================================================

#define NEX_INT8_MIN            NEX_PREPEND_NAMESPACE(LimitConstants::i8min)
#define NEX_INT16_MIN           NEX_PREPEND_NAMESPACE(LimitConstants::i16min)
#define NEX_INT32_MIN           NEX_PREPEND_NAMESPACE(LimitConstants::i32min)
#define NEX_INT64_MIN           NEX_PREPEND_NAMESPACE(LimitConstants::i64min)
#define NEX_INT8_MAX            NEX_PREPEND_NAMESPACE(LimitConstants::i8max)
#define NEX_INT16_MAX           NEX_PREPEND_NAMESPACE(LimitConstants::i16max)
#define NEX_INT32_MAX           NEX_PREPEND_NAMESPACE(LimitConstants::i32max)
#define NEX_INT64_MAX           NEX_PREPEND_NAMESPACE(LimitConstants::i64max)
#define NEX_UINT8_MAX           NEX_PREPEND_NAMESPACE(LimitConstants::u8max)
#define NEX_UINT16_MAX          NEX_PREPEND_NAMESPACE(LimitConstants::u16max)
#define NEX_UINT32_MAX          NEX_PREPEND_NAMESPACE(LimitConstants::u32max)
#define NEX_UINT64_MAX          NEX_PREPEND_NAMESPACE(LimitConstants::u64max)

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

// =================================================================================
// Macro definitions for pointer difference and size limits (C-style)
// =================================================================================

#define NEX_PTRDIFF_MIN         NEX_INTPTR_MIN
#define NEX_PTRDIFF_MAX         NEX_INTPTR_MAX

#define NEX_SIZE_MAX            NEX_PREPEND_NAMESPACE(LimitConstants::sizemax)

// =================================================================================
// Macro definitions for signal atomic type limits (C-style)
// =================================================================================

#define NEX_SIG_ATOMIC_MIN      NEX_PREPEND_NAMESPACE(LimitConstants::i32min)
#define NEX_SIG_ATOMIC_MAX      NEX_PREPEND_NAMESPACE(LimitConstants::i32max)

// =================================================================================
// Macro definitions for wide character limits (C-style)
// =================================================================================

#define NEX_WCHAR_MIN           NEX_PREPEND_NAMESPACE(LimitConstants::wcharmin)
#define NEX_WCHAR_MAX           NEX_PREPEND_NAMESPACE(LimitConstants::wcharmax)

#define NEX_WINT_MIN            NEX_PREPEND_NAMESPACE(LimitConstants::wintmin)
#define NEX_WINT_MAX            NEX_PREPEND_NAMESPACE(LimitConstants::wintmax)

NEX_NAMESPACE_END