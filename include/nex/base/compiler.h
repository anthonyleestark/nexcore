/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once
/**
 * @file  compiler.h
 * @brief Detects the compiler and its compatibility environment.
 *
 * @details
 * This header centralizes compile-time detection of the compiler used to build the code.
 * It identifies supported compiler implementations, such as Clang, GCC, and MSVC,
 * together with relevant compatibility environments and compiler version information.
 *
 * Detection is based on compiler-provided predefined macros and exposes the results through
 * a consistent set of compiler configuration macros.
 *
 * @note
 * Compiler identity and compiler compatibility are separate properties.
 * A compiler may identify as one implementation while providing compatibility
 * with another compiler environment.
 */

// ======================================================================================
// Compiler detection flags
// ======================================================================================

/**
 * Below is a list of macros that are defined for each compiler and C++ version. 
 * Each compiler and C++ version has a corresponding macro that is set to 1 if the compiler or C++ version 
 * is detected, and 0 otherwise.
 */

#define NEX_COMPILER_IS_CLANG_BASED   0   // Compiler is Clang-based (LLVM or Apple Clang)
#define NEX_COMPILER_IS_GCC           0   // Compiler is GCC (GNU Compiler Collection)
#define NEX_COMPILER_IS_MSVC          0   // Compiler is MSVC (Microsoft Visual C++)
#define NEX_COMPILER_IS_INTEL         0   // Compiler is Intel (ICC/ICX)

/**
 * Clang masquerades as GCC on POSIX and as MSVC on Windows, so we define compatibility macros to detect 
 * if we are using a compiler that is compatible with GCC or MSVC, which can be useful for conditional compilation 
 * of code that relies on specific compiler features or behaviors.
 */

#define NEX_COMPILER_GCC_COMPATIBLE   0   // Compiler is GCC-compatible (e.g., Clang on POSIX)
#define NEX_COMPILER_MSVC_COMPATIBLE  0   // Compiler is MSVC-compatible (e.g., Clang on Windows in MSVC compat. mode)

// ======================================================================================
// Compiler detection
// ======================================================================================

// Detects the compiler being used to compile the code by checking for
// predefined macros that are unique to each compiler.
#if defined(__apple_build_version__)
    #undef NEX_COMPILER_IS_CLANG_BASED
    #define NEX_COMPILER_IS_CLANG_BASED 1
    #define NEX_COMPILER_APPLE_CLANG_VER (__apple_build_version__ / 10000)
#elif defined (__clang__)
    #undef NEX_COMPILER_IS_CLANG_BASED
    #define NEX_COMPILER_IS_CLANG_BASED 1
    #define NEX_COMPILER_CLANG_VER (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__)
    #undef NEX_COMPILER_IS_GCC
    #define NEX_COMPILER_IS_GCC 1
    #define NEX_COMPILER_GCC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
#elif defined(_MSC_VER)
    #undef NEX_COMPILER_IS_MSVC
    #define NEX_COMPILER_IS_MSVC 1
    #define NEX_COMPILER_MSVC_VER _MSC_VER
#else
    #error Compiler is not supported or not detected.
    #error Please add support for your compiler in include/nex/base/compiler.h
#endif

/**
 * Defines compatibility macros for GCC and MSVC
 * In case NEX_COMPILER_IS_GCC/MSVC is not true, but the compiler is still compatible with either of them, 
 * it means we are likely using Clang in a compatibility mode
 */

#if defined(__GNUC__)
    #undef NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_COMPILER_GCC_COMPATIBLE 1
    #if !defined(NEX_COMPILER_GCC_VER)
        #define NEX_COMPILER_GCC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
    #endif
#elif defined(_MSC_VER)
    #undef NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_COMPILER_MSVC_COMPATIBLE 1
    #if !defined(NEX_COMPILER_MSVC_VER)
        #define NEX_COMPILER_MSVC_VER _MSC_VER
    #endif
#endif
