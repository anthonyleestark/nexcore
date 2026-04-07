/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      platform.h
 * @defgroup  platform_detection Platform Detection
 * @brief     Detects the target platform and defines macros accordingly for conditional compilation.
 * 
 * @details
 * This section defines macros for various platforms (e.g., Windows, Linux, macOS) and their families (e.g., Apple, BSD, POSIX).
 * It also includes detection for the C++ standard library in use (e.g., glibc) and provides a set of boolean flags 
 * for platform and family detection.
 * The platform detection is based on predefined macros provided by the compiler and the operating system.
 * The macros defined in this section can be used throughout the codebase to write platform-specific code 
 * and to conditionally compile code based on the target platform.
 * 
 * @note Nex-ecosystem only supports Windows, Linux, and macOS for now. Other platforms will result in a compilation error.
 */

/**
 * Below is a list of macros that are defined for each platform. 
 * Each platform has a corresponding macro that is set to 1 if the platform is detected, and 0 otherwise.
 */

#define NEX_PLATFORM_IS_MAC            0
#define NEX_PLATFORM_IS_MACOSX         0
#define NEX_PLATFORM_IS_LINUX          0
#define NEX_PLATFORM_IS_WIN            0
#define NEX_LIBC_IS_GLIBC              0

/**
 * Below is a list of macros that are defined for each platform family. 
 * Each platform family has a corresponding macro that is set to 1 if the platform belongs to that family, and 0 otherwise.
 */

#define NEX_PLATFORM_FAMILY_IS_APPLE   0
#define NEX_PLATFORM_FAMILY_IS_BSD     0
#define NEX_PLATFORM_FAMILY_IS_POSIX   0

////// Platform detection =========================================================

#if defined(ANDROID)
    #error Nex-ecosystem does not support Android for now.
#elif defined(__APPLE__)
	/**
	 * @note TargetConditionals.h is only included after testing for Android because
	 *       some Android builds on Mac have this header available, but it should only
	 *       be used when the target is genuinely an Apple platform.
	 */
	#include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        #error Nex-ecosystem does not support iOS for now.
    #else
        #define NEX_PLATFORM_MAC 1
        #undef NEX_PLATFORM_IS_MAC
        #define NEX_PLATFORM_IS_MAC 1
        // For backwards compatibility.
        #define NEX_PLATFORM_MACOSX 1
        #undef NEX_PLATFORM_IS_MACOSX
        #define NEX_PLATFORM_IS_MACOSX 1
    #endif  // defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#elif defined(__linux__)
    #if !defined(OS_CHROMEOS)
        // Do not define NEX_PLATFORM_LINUX on Chrome OS build.
        #define NEX_PLATFORM_LINUX 1
        #undef NEX_PLATFORM_IS_LINUX
        #define NEX_PLATFORM_IS_LINUX 1
    #else
        #error Nex-ecosystem does not support Chrome OS for now.
    #endif  // !defined(NEX_PLATFORM_CHROMEOS)
    // Include a system header to pull in features.h for glibc/uclibc macros.
    #include <unistd.h>
    #if defined(__GLIBC__) && !defined(__UCLIBC__)
        // We really are using glibc, not uClibc pretending to be glibc.
        #define NEX_LIBC_GLIBC 1
        #undef NEX_LIBC_IS_GLIBC
        #define NEX_LIBC_IS_GLIBC 1
    #endif
#elif defined(_WIN32)
    #define NEX_PLATFORM_WIN 1
    #undef NEX_PLATFORM_IS_WIN
    #define NEX_PLATFORM_IS_WIN 1
#else
    #error Platform is not supported or not detected.
#endif

// For Apple ecosystem
#if defined(NEX_PLATFORM_MAC) || defined(NEX_PLATFORM_IOS)
    #define NEX_PLATFORM_APPLE 1
    #undef NEX_PLATFORM_FAMILY_IS_APPLE
    #define NEX_PLATFORM_FAMILY_IS_APPLE 1
#endif

// For access to standard BSD features, use NEX_PLATFORM_BSD instead of a
// more specific macro.
#if defined(NEX_PLATFORM_FREEBSD) || defined(NEX_PLATFORM_NETBSD) || defined(NEX_PLATFORM_OPENBSD)
    #define NEX_PLATFORM_BSD 1
    #undef NEX_PLATFORM_FAMILY_IS_BSD
    #define NEX_PLATFORM_FAMILY_IS_BSD 1
#endif

// For access to standard POSIXish features, use NEX_PLATFORM_POSIX instead of a
// more specific macro.
#if defined(NEX_PLATFORM_AIX) || defined(NEX_PLATFORM_ANDROID) || defined(NEX_PLATFORM_ASMJS) ||  \
    defined(NEX_PLATFORM_FREEBSD) || defined(NEX_PLATFORM_IOS) || defined(NEX_PLATFORM_LINUX) ||  \
    defined(NEX_PLATFORM_CHROMEOS) || defined(NEX_PLATFORM_MAC) || defined(NEX_PLATFORM_NACL) ||  \
    defined(NEX_PLATFORM_NETBSD) || defined(NEX_PLATFORM_OPENBSD) || defined(NEX_PLATFORM_QNX) || \
    defined(NEX_PLATFORM_SOLARIS)
    #define NEX_PLATFORM_POSIX 1
    #undef NEX_PLATFORM_FAMILY_IS_POSIX
    #define NEX_PLATFORM_FAMILY_IS_POSIX 1
#endif
