/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    language.h
 * @brief   Detects the active C++ language standard and language features.
 *
 * @details
 * This header centralizes compile-time detection of the active C++ language standard
 * and relevant language-level capabilities.
 *
 * It provides a consistent set of macros for checking the supported C++ standard version
 * and the availability of language features required or optionally used by NexCore.
 *
 * Detection is based primarily on standard language feature-test macros and compiler-provided
 * language-version information.
 *
 * @note
 * Language features are distinct from compiler-specific extensions and standard library
 * implementation capabilities.
 */

// ======================================================================================
// C++ language standard version macros
// ======================================================================================

/**
 * Below is a list of macros that define the version numbers for each C++ language standard. 
 * These macros can be used to compare against the __cplusplus macro to determine the C++ version being used 
 * to compile the code.
 */

#define NEX_CXX_STD_98                  199711L
#define NEX_CXX_STD_03                  199711L
#define NEX_CXX_STD_11                  201103L
#define NEX_CXX_STD_14                  201402L
#define NEX_CXX_STD_17                  201703L
#define NEX_CXX_STD_20                  202002L
#define NEX_CXX_STD_23                  202302L
#define NEX_CXX_STD_26                  0xFFFFFL    // (not yet standardized)

// ======================================================================================
// C++ version detection
// ======================================================================================

// Determines the active C++ version by checking the __cplusplus macro, 
// which is defined by the compiler to indicate the C++ standard version being used.
#ifdef __cplusplus
    #if defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus)
        #define NEX_CXX_VER _MSVC_LANG     // override with MSVC version
    #else
        // Use the value of __cplusplus provided by the compiler, 
        // which should reflect the C++ version being used.
        #define NEX_CXX_VER __cplusplus
    #endif
#else
    #define NEX_CXX_VER 0L                 // no C++ support
#endif

// Determines the C++ standard version based on the detected C++ version.
#if NEX_CXX_VER <= NEX_CXX_STD_11
    #define NEX_CXX_STD_VER 11
#elif NEX_CXX_VER <= NEX_CXX_STD_14
    #define NEX_CXX_STD_VER 14
#elif NEX_CXX_VER <= NEX_CXX_STD_17
    #define NEX_CXX_STD_VER 17
#elif NEX_CXX_VER <= NEX_CXX_STD_20
    #define NEX_CXX_STD_VER 20
#elif NEX_CXX_VER <= NEX_CXX_STD_23
    #define NEX_CXX_STD_VER 23
#elif NEX_CXX_VER <= NEX_CXX_STD_26
    #define NEX_CXX_STD_VER 26
#endif

// Checks if it has C++17 or later
#ifndef NEX_HAS_CXX17
    #if NEX_CXX_VER > NEX_CXX_STD_14
        #define NEX_HAS_CXX17 1
    #else
        #define NEX_HAS_CXX17 0
    #endif
#endif

// Checks if it has C++20 or later
#ifndef NEX_HAS_CXX20
    #if NEX_HAS_CXX17 && NEX_CXX_VER > NEX_CXX_STD_17
        #define NEX_HAS_CXX20 1
    #else
        #define NEX_HAS_CXX20 0
    #endif
#endif

// Checks if it has C++23 or later
#ifndef NEX_HAS_CXX23
    #if NEX_HAS_CXX20 && NEX_CXX_VER > NEX_CXX_STD_20
        #define NEX_HAS_CXX23 1
    #else
        #define NEX_HAS_CXX23 0
    #endif
#endif

// Checks if it has C++26 or later
#ifndef NEX_HAS_CXX26
    #if NEX_HAS_CXX23 && NEX_CXX_VER > NEX_CXX_STD_23
        #define NEX_HAS_CXX26 1
    #else
        #define NEX_HAS_CXX26 0
    #endif
#endif

// Does not support C++17 and earlier
#if !NEX_HAS_CXX20
    #error Nex-ecosystem only supports C++20 or later.
#endif

// C++23 must imply C++20
#if NEX_HAS_CXX23 && !NEX_HAS_CXX20
    #error NEX_HAS_CXX23 must imply NEX_HAS_CXX20.
#endif

// C++26 must imply C++23
#if NEX_HAS_CXX26 && !NEX_HAS_CXX23
    #error NEX_HAS_CXX26 must imply NEX_HAS_CXX23.
#endif

// =================================================================================
// Detects support for C++ language features/types
// =================================================================================

#if NEX_HAS_CXX20 && defined(__cpp_char8_t)
    #define NEX_HAS_BUILTIN_CHAR8_T 1
#else  // Does not support char8_t
    #define NEX_HAS_BUILTIN_CHAR8_T 0
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

// =================================================================================
// Extern "C" macros for C++ code that needs to be callable from C
// =================================================================================

#define NEX_EXTERN_C                extern "C"
#define NEX_EXTERN_C_BEGIN          extern "C" {
#define NEX_EXTERN_C_END            }
#define NEX_EXTERN_C_BLOCK(...)     NEX_EXTERN_C_BEGIN __VA_ARGS__ NEX_EXTERN_C_END

// ================================================================================
// Extern "C++" macros for C code that needs to be callable from C++
// ================================================================================

#define NEX_EXTERN_CPP              extern "C++"
#define NEX_EXTERN_CPP_BEGIN        extern "C++" {
#define NEX_EXTERN_CPP_END          }
#define NEX_EXTERN_CPP_BLOCK(...)   NEX_EXTERN_CPP_BEGIN __VA_ARGS__ NEX_EXTERN_CPP_END
