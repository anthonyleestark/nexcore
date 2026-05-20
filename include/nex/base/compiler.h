/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    compiler.h
 * @brief   Detects the compiler and C++ version being used to compile the code, and defines macros for 
 *          conditional compilation based on the compiler and C++ version.
 * 
 * @details
 * This header centralizes compile-time compiler and language-version detection and exposes the result as a small
 * set of macros.
 * It distinguishes specific compiler targets such as Clang, GCC, and MSVC, compatibility modes for GCC and MSVC,
 * and the active C++ language version.
 * Detection is based on compiler-provided predefined macros so the rest of the codebase can rely on a single,
 * consistent interface for conditional compilation.
 * 
 * @note 
 * Only C++20 and later are supported; earlier language modes fail fast at compile time.
 */

// ======================================================================================
// Compiler detection flags
// ======================================================================================

/**
 * Below is a list of macros that are defined for each compiler and C++ version. 
 * Each compiler and C++ version has a corresponding macro that is set to 1 if the compiler or C++ version 
 * is detected, and 0 otherwise.
 */

#define NEX_COMPILER_IS_CLANG          0    // Compiler is Clang (LLVM)
#define NEX_COMPILER_IS_GCC            0    // Compiler is GCC (GNU Compiler Collection)
#define NEX_COMPILER_IS_MSVC           0    // Compiler is MSVC (Microsoft Visual C++)

/**
 * Clang masquerades as GCC on POSIX and as MSVC on Windows, so we define compatibility macros to detect 
 * if we are using a compiler that is compatible with GCC or MSVC, which can be useful for conditional compilation 
 * of code that relies on specific compiler features or behaviors.
 */

#define NEX_COMPILER_GCC_COMPATIBLE    0    // Compiler is GCC-compatible (e.g., Clang on POSIX)
#define NEX_COMPILER_MSVC_COMPATIBLE   0    // Compiler is MSVC-compatible (e.g., Clang on Windows in MSVC compat. mode)

/**
 * Below is a list of macros that define the version numbers for each C++ language standard. 
 * These macros can be used to compare against the __cplusplus macro to determine the C++ version being used 
 * to compile the code.
 */

#define NEX_CXX98_VER_NUMBER           199711L
#define NEX_CXX03_VER_NUMBER           199711L
#define NEX_CXX11_VER_NUMBER           201103L
#define NEX_CXX14_VER_NUMBER           201402L
#define NEX_CXX17_VER_NUMBER           201703L
#define NEX_CXX20_VER_NUMBER           202002L
#define NEX_CXX23_VER_NUMBER           202302L
#define NEX_CXX26_VER_NUMBER           0xFFFFFL    // (not yet standardized)

// ======================================================================================
// Compiler detection
// ======================================================================================

// Detect the compiler being used to compile the code by checking for predefined macros 
// that are unique to each compiler.
#if defined (__clang__)
    #define NEX_COMPILER_CLANG    __clang_major__
    #undef NEX_COMPILER_IS_CLANG
    #define NEX_COMPILER_IS_CLANG 1
#elif defined(__GNUC__)
    #define NEX_COMPILER_GCC    __GNUC__
    #undef NEX_COMPILER_IS_GCC
    #define NEX_COMPILER_IS_GCC 1
#elif defined(_MSC_VER)
    #define NEX_COMPILER_MSVC    _MSC_VER
    #undef NEX_COMPILER_IS_MSVC
    #define NEX_COMPILER_IS_MSVC 1
#else
    #error Compiler is not supported or not detected.
#endif

/**
 * Define compatibility macros for GCC and MSVC
 * In case NEX_COMPILER_IS_GCC/MSVC is not true, but the compiler is still compatible with either of them, 
 * it means we are likely using Clang in a compatibility mode
 */

#if defined(__GNUC__)
    #undef NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_COMPILER_GCC_COMPATIBLE 1
    #if !defined(NEX_COMPILER_GCC)
        #define NEX_COMPILER_GCC  __GNUC__
    #endif
#elif defined(_MSC_VER)
    #undef NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_COMPILER_MSVC_COMPATIBLE 1
    #if !defined(NEX_COMPILER_MSVC)
        #define NEX_COMPILER_MSVC  _MSC_VER
    #endif
#endif

// ======================================================================================
// C++ version detection
// ======================================================================================

// Determine the active C++ version by checking the __cplusplus macro, 
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

// Check if it has C++17 or later
#ifndef NEX_HAS_CXX17
    #if NEX_CXX_VER > NEX_CXX14_VER_NUMBER
        #define NEX_HAS_CXX17 1
    #else
        #define NEX_HAS_CXX17 0
    #endif
#endif

// Check if it has C++20 or later
#ifndef NEX_HAS_CXX20
    #if NEX_HAS_CXX17 && NEX_CXX_VER > NEX_CXX17_VER_NUMBER
        #define NEX_HAS_CXX20 1
    #else
        #define NEX_HAS_CXX20 0
    #endif
#endif

// Check if it has C++23 or later
#ifndef NEX_HAS_CXX23
    #if NEX_HAS_CXX20 && NEX_CXX_VER > NEX_CXX20_VER_NUMBER
        #define NEX_HAS_CXX23 1
    #else
        #define NEX_HAS_CXX23 0
    #endif
#endif

// Check if it has C++26 or later
#ifndef NEX_HAS_CXX26
    #if NEX_HAS_CXX23 && NEX_CXX_VER > NEX_CXX23_VER_NUMBER
        #define NEX_HAS_CXX26 1
    #else
        #define NEX_HAS_CXX26 0
    #endif
#endif

// Do not support C++17 and earlier
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
