/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  libc.h
 * @brief Detects the C runtime library used by the target environment.
 *
 * @details
 * This header centralizes compile-time detection of the C runtime library and related runtime
 * environment characteristics.
 *
 * It identifies supported runtime implementations and exposes the results through a consistent
 * set of configuration macros.
 *
 * Detection is based on compiler-, platform-, and runtime-provided predefined macros.
 *
 * @note
 * C runtime detection is distinct from compiler identity and C++ ABI detection.
 * A compiler may target different runtime environments depending on the toolchain configuration.
 */

#include "nex/base/platform.h"
#include "nex/base/compiler_features.h"

// =================================================================================
// C library flags
// =================================================================================

/**
 * Below is a list of flags that are defined for each possible C library implementation.
 * Each flag is set to 1 if the corresponding C library is detected, and 0 otherwise.
 */

#define NEX_LIBC_IS_GLIBC              0
#define NEX_LIBC_IS_UCLIBC             0

// =================================================================================
// C library detection
// =================================================================================

// Detects which libc we're using if we're on Linux.
#if NEX_PLATFORM_IS_LINUX
    // Include a system header to pull in features.h for glibc/uclibc macros.
    #include <unistd.h>
    #if defined(__GLIBC__) && !defined(__UCLIBC__)
        // We really are using glibc, not uClibc pretending to be glibc.
        #undef NEX_LIBC_IS_GLIBC
        #define NEX_LIBC_IS_GLIBC 1
    #elif defined(__UCLIBC__)
        #undef NEX_LIBC_IS_UCLIBC
        #define NEX_LIBC_IS_UCLIBC 1
    #endif
#endif  // ^^NEX_PLATFORM_IS_LINUX

#if (defined(NEX_PLATFORM_LINUX) || defined(__AMDGPU__) || defined(__NVPTX__)) && NEX_HAS_INCLUDE(<features.h>)
    #include <features.h>
    #if NEX_LIBC_IS_GLIBC && defined(__GLIBC_PREREQ)
        #define NEX_LIBC_GLIBC_PREREQ(major, minor) __GLIBC_PREREQ(major, minor)
    #else
        #define NEX_LIBC_GLIBC_PREREQ(major, minor) 0
    #endif
#else
    #define NEX_LIBC_GLIBC_PREREQ(major, minor) 0
#endif  // ^^NEX_LIBC_GLIBC_PREREQ
