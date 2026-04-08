/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      compiler.h
 * @defgroup  compiler_detection Compiler and C++ Version Detection
 * @brief     Detects the compiler and C++ version being used to compile the code, and defines macros for conditional 
 *            compilation based on the compiler and C++ version.
 * 
 * @details
 * This section defines macros for various compilers (e.g., GCC, MSVC) and their versions, 
 * as well as macros for the C++ language standard version being used (e.g., C++11, C++14, C++17, C++20).
 * It also provides a set of boolean flags for compiler and C++ version detection.
 * The compiler and C++ version detection is based on predefined macros provided by the compiler.
 * 
 * @note Nex-ecosystem only supports C++20 or later. Compilers that do not support C++20 will result in a compilation error.
 */

/**
 * Below is a list of macros that are defined for each compiler and C++ version. 
 * Each compiler and C++ version has a corresponding macro that is set to 1 if the compiler or C++ version is detected, and 0 otherwise.
 */

#define NEX_COMPILER_IS_GCC            0
#define NEX_COMPILER_IS_MSVC           0

/**
 * Below is a list of macros that define the version numbers for each C++ language standard. 
 * These macros can be used to compare against the __cplusplus macro to determine the C++ version being used to compile the code.
 */

#define NEX_CXX_98_VER_NUMBER          199711L
#define NEX_CXX_03_VER_NUMBER          199711L
#define NEX_CXX_11_VER_NUMBER          201103L
#define NEX_CXX_14_VER_NUMBER          201402L
#define NEX_CXX_17_VER_NUMBER          201703L
#define NEX_CXX_20_VER_NUMBER          202002L
#define NEX_CXX_23_VER_NUMBER          202302L
#define NEX_CXX_26_VER_NUMBER          0xFFFFFL    // (not yet standardized)

/**
 * @section Compiler detection macros
 * @brief   Define macros for detecting the compiler being used to compile the code
 * @note
 * The NEX_COMPILER_GCC and NEX_COMPILER_MSVC macros are defined to 1 if the corresponding compiler is detected, 
 * and the NEX_COMPILER_IS_GCC and NEX_COMPILER_IS_MSVC macros are defined to 1 if the corresponding compiler is 
 * detected, and 0 otherwise.
 * Clang masquerades as GCC on POSIX and as MSVC on Windows.
 */
#if defined(__GNUC__)
    #define NEX_COMPILER_GCC 1
    #undef NEX_COMPILER_IS_GCC
    #define NEX_COMPILER_IS_GCC 1
#elif defined(_MSC_VER)
    #define NEX_COMPILER_MSVC 1
    #undef NEX_COMPILER_IS_MSVC
    #define NEX_COMPILER_IS_MSVC 1
#else
    #error Compiler is not supported or not detected.
#endif

/**
 * @section C++ version detection macros
 * @brief   Define macros for detecting the C++ version being used to compile the code
 * @note
 * The NEX_CXX_VER macro is defined to the value of the __cplusplus macro, or the _MSVC_LANG macro if using MSVC.
 */
#ifdef __cplusplus
    #if defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus)
        #define NEX_CXX_VER _MSVC_LANG     // override with MSVC version
    #else
        #define NEX_CXX_VER __cplusplus
    #endif
#else
    #define NEX_CXX_VER 0L                 // no C++ support
#endif

// Check if it has C++ 17 or later
#ifndef NEX_HAS_CXX17
    #if NEX_CXX_VER > NEX_CXX_14_VER_NUMBER
        #define NEX_HAS_CXX17 1
    #else
        #define NEX_HAS_CXX17 0
    #endif
#endif

// Check if it has C++ 20 or later
#ifndef NEX_HAS_CXX20
    #if NEX_HAS_CXX17 && NEX_CXX_VER > NEX_CXX_17_VER_NUMBER
        #define NEX_HAS_CXX20 1
    #else
        #define NEX_HAS_CXX20 0
    #endif
#endif

// Check if it has C++ 23 or later
#ifndef NEX_HAS_CXX23
    #if NEX_HAS_CXX20 && NEX_CXX_VER > NEX_CXX_20_VER_NUMBER
        #define NEX_HAS_CXX23 1
    #else
        #define NEX_HAS_CXX23 0
    #endif
#endif

// Check if it has C++ 26 or later
#ifndef NEX_HAS_CXX26
    #if NEX_HAS_CXX23 && NEX_CXX_VER > NEX_CXX_23_VER_NUMBER
        #define NEX_HAS_CXX26 1
    #else
        #define NEX_HAS_CXX26 0
    #endif
#endif

// Do not support C++ 17 and earlier
#if !NEX_HAS_CXX20
    #error Nex-ecosystem only supports C++ 20 or later.
#endif

// C++ 23 must imply C++ 20
#if NEX_HAS_CXX23 && !NEX_HAS_CXX20
    #error NEX_HAS_CXX23 must imply NEX_HAS_CXX20.
#endif

// C++ 26 must imply C++ 23
#if NEX_HAS_CXX26 && !NEX_HAS_CXX23
    #error NEX_HAS_CXX26 must imply NEX_HAS_CXX23.
#endif
