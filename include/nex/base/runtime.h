/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  runtime.h
 * @brief Detects and configures the C runtime library in use (UCRT or MSVCRT) for Windows platforms.
 *
 * @details
 * This header provides macros to detect and configure the C runtime library used by the application.
 * It supports automatic detection as well as user overrides for UCRT and MSVCRT.
 */

#include "nex/base/abi.h"

// Windows uses the VC Runtime for the Microsoft ABI
// if it is not explicitly disabled via preprocessor definition (NEX_NO_VCRUNTIME).
#if defined(NEX_ABI_CXX_MICROSOFT) && !defined(NEX_NO_VCRUNTIME)
    #define NEX_RUNTIME_VCRUNTIME
#endif  // ^^NEX_RUNTIME_VCRUNTIME

/**
 * @note
 * User can override the detected C runtime by defining NEX_CRT_FORCE_UCRT or NEX_CRT_FORCE_MSVCRT
 * before including this header. This allows the user to explicitly specify the C runtime to be used,
 * overriding the automatic detection logic.
 * Defining these macros will take precedence over the automatic C runtime detection logic.
 * If neither macro is defined, the C runtime will be automatically detected based on platform
 * and compiler characteristics.
 */

#if !defined(NEX_CRT_FORCE_UCRT)
    #define NEX_CRT_FORCE_UCRT 0
#endif  // ^^!defined(NEX_CRT_FORCE_UCRT)

#if !defined(NEX_CRT_FORCE_MSVCRT)
    #define NEX_CRT_FORCE_MSVCRT 0
#endif  // ^^!defined(NEX_CRT_FORCE_MSVCRT)

// =================================================================================
// Detects the C runtime library in use (UCRT or MSVCRT)
// =================================================================================

#if NEX_CRT_FORCE_UCRT && NEX_CRT_FORCE_MSVCRT
    // Conflicting CRT overrides
    #error Cannot force both UCRT and MSVCRT simultaneously.

#elif NEX_CRT_FORCE_UCRT
    // User explicitly requested UCRT
    #define NEX_CRT_UCRT

#elif NEX_CRT_FORCE_MSVCRT
    // User explicitly requested MSVCRT
    #define NEX_CRT_MSVCRT

#elif defined(__MINGW32__)
    // Requires additional detection to distinguish
    // UCRT and MSVCRT configurations.

#elif NEX_PLATFORM_IS_WINDOWS && NEX_COMPILER_MSVC_COMPATIBLE
    // Modern MSVC toolchains normally use UCRT.
    #define NEX_CRT_UCRT

#endif  // ^^C runtime detection
