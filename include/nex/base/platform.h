/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  platform.h
 * @brief Detects the target operating system and platform family.
 *
 * @details
 * This header centralizes compile-time detection of the target operating system and its broader platform family.
 * It identifies supported platforms such as Windows, Linux, and macOS, together with platform families such as
 * Apple, BSD, and POSIX.
 *
 * Detection is based on compiler- and platform-provided predefined macros and exposes the results through
 * a consistent set of configuration macros.
 *
 * @note
 * Platform-family detection does not imply that the corresponding platform is supported by NexCore.
 */

// ======================================================================================
// Platform target and broader family flags
// ======================================================================================

/**
 * Below is a list of flags that are defined for each platform. 
 * Each platform has a corresponding flag that is set to 1 if the target platform is detected, and 0 otherwise.
 */

#define NEX_PLATFORM_IS_MAC                 0
#define NEX_PLATFORM_IS_MACOSX              0
#define NEX_PLATFORM_IS_LINUX               0
#define NEX_PLATFORM_IS_WINDOWS             0

#define NEX_PLATFORM_IS_ANDROID             0
#define NEX_PLATFORM_IS_IOS                 0

#define NEX_PLATFORM_IS_CHROMEOS            0
#define NEX_PLATFORM_IS_UNIX                0
#define NEX_PLATFORM_IS_AIX                 0
#define NEX_PLATFORM_IS_SOLARIS             0
#define NEX_PLATFORM_IS_FREEBSD             0
#define NEX_PLATFORM_IS_NETBSD              0
#define NEX_PLATFORM_IS_OPENBSD             0
#define NEX_PLATFORM_IS_QNX                 0
#define NEX_PLATFORM_IS_ASMJS               0
#define NEX_PLATFORM_IS_WASM                0
#define NEX_PLATFORM_IS_NACL                0
#define NEX_PLATFORM_IS_HURD                0
#define NEX_PLATFORM_IS_HAIKU               0
#define NEX_PLATFORM_IS_BEOS                0

/**
 * Below is a list of flags that are defined for each platform family. 
 * Each family has a corresponding flag that is set to 1 if the target platform belongs to that family, and 0 otherwise.
 */

#define NEX_PLATFORM_FAMILY_IS_APPLE        0
#define NEX_PLATFORM_FAMILY_IS_BSD          0
#define NEX_PLATFORM_FAMILY_IS_POSIX        0
#define NEX_PLATFORM_FAMILY_IS_UNIX         0
#define NEX_PLATFORM_FAMILY_IS_MICROSOFT    0

// ======================================================================================
// Platform detection
// ======================================================================================

#if defined(ANDROID)
    #error Nex-ecosystem does not support Android for now.
#elif defined(__APPLE__)
    /**
     * @note 
     * TargetConditionals.h is only included after testing for Android because some Android builds on Mac 
     * have this header available, but it should only be used when the target is genuinely an Apple platform.
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
#elif defined(_WIN32) || defined(_WIN64)
    #define NEX_PLATFORM_WINDOWS 1
    #undef NEX_PLATFORM_IS_WINDOWS
    #define NEX_PLATFORM_IS_WINDOWS 1
    #if defined(_WIN64)
        #define NEX_PLATFORM_WINDOWS_64 1
    #else  // Windows 32-bit
        #define NEX_PLATFORM_WINDOWS_32 1
    #endif
#else
    #error Platform is not supported or not detected.
    #error Please add support for your platform in include/nex/base/platform.h
#endif

// ======================================================================================
// Platform family detection
// ======================================================================================

// For Microsoft ecosystem
#if defined(NEX_PLATFORM_WINDOWS)
    #define NEX_PLATFORM_MICROSOFT 1
    #undef NEX_PLATFORM_FAMILY_IS_MICROSOFT
    #define NEX_PLATFORM_FAMILY_IS_MICROSOFT 1
#endif

// For Apple ecosystem
#if defined(NEX_PLATFORM_MAC) || defined(NEX_PLATFORM_IOS)
    #define NEX_PLATFORM_APPLE 1
    #undef NEX_PLATFORM_FAMILY_IS_APPLE
    #define NEX_PLATFORM_FAMILY_IS_APPLE 1
#endif

// For access to standard BSD features, use NEX_PLATFORM_BSD instead of a more specific macro.
#if defined(NEX_PLATFORM_FREEBSD) || defined(NEX_PLATFORM_NETBSD) || defined(NEX_PLATFORM_OPENBSD)
    #define NEX_PLATFORM_BSD 1
    #undef NEX_PLATFORM_FAMILY_IS_BSD
    #define NEX_PLATFORM_FAMILY_IS_BSD 1
#endif

// For access to standard POSIXish features, use NEX_PLATFORM_POSIX instead of a more specific macro.
#if defined(NEX_PLATFORM_AIX) || defined(NEX_PLATFORM_ANDROID) || defined(NEX_PLATFORM_ASMJS) ||  \
    defined(NEX_PLATFORM_FREEBSD) || defined(NEX_PLATFORM_IOS) || defined(NEX_PLATFORM_LINUX) ||  \
    defined(NEX_PLATFORM_CHROMEOS) || defined(NEX_PLATFORM_MAC) || defined(NEX_PLATFORM_NACL) ||  \
    defined(NEX_PLATFORM_NETBSD) || defined(NEX_PLATFORM_OPENBSD) || defined(NEX_PLATFORM_QNX) || \
    defined(NEX_PLATFORM_SOLARIS)
    #define NEX_PLATFORM_POSIX 1
    #undef NEX_PLATFORM_FAMILY_IS_POSIX
    #define NEX_PLATFORM_FAMILY_IS_POSIX 1
#endif

// For Unix-like platforms
#if defined(NEX_PLATFORM_APPLE) || defined(NEX_PLATFORM_BSD) || defined(NEX_PLATFORM_POSIX)
    #define NEX_PLATFORM_FAMILY_UNIX 1
    #undef NEX_PLATFORM_FAMILY_IS_UNIX
    #define NEX_PLATFORM_FAMILY_IS_UNIX 1
#endif
