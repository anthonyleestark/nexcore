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
#include "nex/base/data_model.h"

/**
 * @note
 * User can override the detected ABI by defining NEX_ABI_FORCE_ITANIUM or NEX_ABI_FORCE_MICROSOFT
 * before including this header. This allows the user to explicitly specify the ABI to be used,
 * overriding the automatic detection logic.
 * Defining these macros will take precedence over the automatic ABI detection logic.
 * If neither macro is defined, the ABI will be automatically detected based on platform
 * and compiler characteristics.
 */

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
    // Conflicting ABI overrides
    #error Cannot force both Itanium and Microsoft ABIs simultaneously.

#elif NEX_ABI_FORCE_ITANIUM
    // User explicitly requested Itanium ABI
    #define NEX_ABI_CXX_ITANIUM

#elif NEX_ABI_FORCE_MICROSOFT
    // User explicitly requested Microsoft ABI
    #define NEX_ABI_CXX_MICROSOFT

#else
    // Windows uses the Microsoft ABI by default.
    // This is a heuristic and may not cover all cases, especially with non-MSVC compilers on Windows.
    #if NEX_PLATFORM_IS_WINDOWS && NEX_COMPILER_MSVC_COMPATIBLE
        #define NEX_ABI_CXX_MICROSOFT

    // GCC/Clang compilers define __GXX_ABI_VERSION when using the Itanium ABI.
    // This covers Linux, BSD, and most Unix-like platforms using GCC/Clang.
    #elif defined(__GXX_ABI_VERSION)
        #define NEX_ABI_CXX_ITANIUM

    // 32-bit ARM uses the Itanium ABI with a few differences, mainly in calling conventions and data layout.
    // 64-bit ARM on Apple platforms also uses the Itanium ABI with Apple-specific differences.
    #elif defined(__arm__) || NEX_PLATFORM_FAMILY_IS_APPLE && defined(NEX_ARCH_CPU_ARM64)
        #define NEX_ABI_CXX_ITANIUM_WITH_ARM_DIFFERENCES

    // Non-Apple 64-bit ARM uses the vanilla Itanium ABI.
    #elif defined(NEX_ARCH_CPU_ARM64)
        #define NEX_ABI_CXX_ITANIUM

    // IBM XL C/C++ compiler uses its own ABI.
    // This is just a placeholder comment for the IBM XL C/C++ ABI detection.
    #elif defined(__IBMCPP__) || defined(__ibmxl__) || defined(__xlC__)
        #define NEX_ABI_CXX_IBM_XL

    #else
        #error Unable to detect C++ ABI for the current platform and compiler.
        #error Please add support for your target ABI in include/nex/base/abi.h.
    #endif
#endif  // ^^ABI detection
