/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  abi.h
 * @brief Detects and configures the target C++ ABI.
 *
 * @details
 * This header provides compile-time detection and configuration of the Application Binary Interface (ABI)
 * used by the target toolchain.
 *
 * It identifies supported C++ ABI families, such as the Microsoft C++ ABI and the Itanium C++ ABI,
 * and exposes the results through a consistent set of configuration macros.
 *
 * The detection logic may use compiler-provided information, target characteristics, and explicit
 * configuration overrides where necessary.
 *
 * @note
 * ABI detection is toolchain- and target-dependent.
 * Unsupported or ambiguous ABI configurations should be reported explicitly rather than inferred
 * from unrelated platform characteristics.
 */

#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"

#if !defined(NEX_ABI_FORCE_ITANIUM)
    #define NEX_ABI_FORCE_ITANIUM 0
#endif  // ^^!defined(NEX_ABI_FORCE_ITANIUM)

#if !defined(NEX_ABI_FORCE_MICROSOFT)
    #define NEX_ABI_FORCE_MICROSOFT 0
#endif  // ^^!defined(NEX_ABI_FORCE_MICROSOFT)

/**
 * FIXME:
 * ABI detection should be done via compiler builtin macros.
 * This is just a placeholder until proper compiler builtin detection is implemented.
 * Currently, the ABI is determined based on platform and architecture macros.
 * For now, we assume that Windows compilers pretending to be MSVC++ target the Microsoft ABI,
 * and allow the user to explicitly specify the ABI to handle cases where this heuristic falls short.
 */
#if NEX_ABI_FORCE_ITANIUM && NEX_ABI_FORCE_MICROSOFT
    #error Cannot force both Itanium and Microsoft ABIs simultaneously.
#elif NEX_ABI_FORCE_ITANIUM
    #define NEX_ABI_ITANIUM
#elif NEX_ABI_FORCE_MICROSOFT
    #define NEX_ABI_MICROSOFT
#else
    // Windows uses the Microsoft ABI by default.
    #if NEX_PLATFORM_IS_WINDOWS && NEX_COMPILER_MSVC_COMPATIBLE
        #define NEX_ABI_MICROSOFT
    // 32-bit ARM uses the Itanium ABI with a few differences,
    // and so does 64-bit ARM on Apple platforms.
    #elif defined(__arm__) || NEX_PLATFORM_FAMILY_IS_APPLE && defined(NEX_ARCH_CPU_ARM64)
        #define NEX_ABI_ITANIUM_WITH_ARM_DIFFERENCES
    // Non-Apple 64-bit ARM uses the vanilla Itanium ABI
    #elif defined(NEX_ARCH_CPU_ARM64)
        #define NEX_ABI_ITANIUM
    #else
        #error Unsupported architecture for ABI detection.
    #endif
#endif  // ^^ABI detection

// Windows uses the VC Runtime for the Microsoft ABI
// if it is not explicitly disabled via preprocessor definition (NEX_NO_VCRUNTIME).
#if defined(NEX_ABI_MICROSOFT) && !defined(NEX_NO_VCRUNTIME)
    #define NEX_ABI_VCRUNTIME
#endif  // ^^NEX_ABI_VCRUNTIME

#if NEX_PLATFORM_IS_WINDOWS
    // Both MinGW and native MSVC provide a "MSVC"-like environment
    #define NEX_MSVCRT_LIKE
    // If MINGW not explicitly detected, assume using MS C runtime
    // only if a MS compatibility version is specified.
    #if NEX_COMPILER_MSVC_COMPATIBLE && !defined(__MINGW32__)
        #define NEX_MSVCRT
    #endif
#endif
