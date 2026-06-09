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

// ================================================================================
// Compiler-specific attribute and feature detection macros
// ================================================================================

/**
 * @def NEX_HAS_CPP_ATTRIBUTE(x)
 * @brief Check if a C++ attribute is supported by the compiler
 * 
 * @details
 * This is a wrapper around `__has_cpp_attribute`, which can be used to test for the presence of an attribute. 
 * In case the compiler does not support this macro it will simply evaluate to 0.
 * 
 * @see https://wg21.link/sd6#testing-for-the-presence-of-an-attribute-__has_cpp_attribute
 * @see https://wg21.link/cpp.cond#:__has_cpp_attribute
 */
#if defined(__has_cpp_attribute)
    #define NEX_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else  // Compiler does not support __has_cpp_attribute
    #define NEX_HAS_CPP_ATTRIBUTE(x) 0
#endif  // defined(__has_cpp_attribute)

/**
 * @def NEX_HAS_BUILTIN(x)
 * @brief Check if a compiler builtin is available
 * 
 * @details
 * This is a wrapper around `__has_builtin`, similar to NEX_HAS_CPP_ATTRIBUTE.
 * If the compiler does not support `__has_builtin`, this macro evaluates to 0.
 */
#if defined(__has_builtin)
    #define NEX_HAS_BUILTIN(x) __has_builtin(x)
#else  // Compiler does not support __has_builtin
    #define NEX_HAS_BUILTIN(x) 0
#endif  // defined(__has_builtin)

/**
 * @def NEX_HAS_FEATURE(x)
 * @brief Check if a compiler feature is available
 * 
 * @details
 * This is a wrapper around `__has_feature`, similar to NEX_HAS_CPP_ATTRIBUTE.
 * If the compiler does not support `__has_feature`, this macro evaluates to 0.
 */

// __has_feature is not available in MSVC.
#if !defined(__has_feature)
    #define __has_feature(x) 0
#endif  // !defined(__has_feature)

#if !defined(NEX_HAS_FEATURE)
    #define NEX_HAS_FEATURE(x) __has_feature(x)
#endif  // !defined(NEX_HAS_FEATURE)

/**
 * @def NEX_HAS_ATTRIBUTE(x)
 * @brief Check if a compiler attribute is supported by the compiler
 * 
 * @details
 * This is a wrapper around `__has_attribute`, similar to NEX_HAS_CPP_ATTRIBUTE.
 * If the compiler does not support `__has_attribute`, this macro evaluates to 0.
 */

// __has_attribute is not available in MSVC.
#if !defined(__has_attribute)
    #define __has_attribute(x) 0
#endif  // !defined(__has_attribute)

#if !defined(NEX_HAS_ATTRIBUTE)
    #define NEX_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif  // !defined(NEX_HAS_ATTRIBUTE)

// =================================================================================
// Compiler-specific type definitions and feature detection
// =================================================================================

#if defined(__SIZEOF_INT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_INT128 1
#else // Compiler does not support __int128
    #define NEX_HAS_BUILTIN_INT128 0
#endif  // ^^NEX_HAS_BUILTIN_INT128

#if defined(__FLT16_DIG__) || defined(__fp16) || defined(_Float16)
    #define NEX_HAS_BUILTIN_FLOAT16 1
    #if defined(__fp16) && (NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG)
        // Clang and GCC support __fp16 as a native 16-bit floating-point type, 
        // but only as a storage-only format and only on certain targets (e.g., ARM)
        using __float16_t = __fp16;
    #else  // _Float16 
        using __float16_t = _Float16;
    #endif
#else  // Compiler does not support __float16_t
    #define NEX_HAS_BUILTIN_FLOAT16 0
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else  // Compiler does not support __float128
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

#if NEX_COMPILER_IS_MSVC
    #define NEX_SIZEOF_LONG_DOUBLE 8
#elif defined(__SIZEOF_LONG_DOUBLE__)
    // GCC and Clang provide this exact size macro in bytes
    #define NEX_SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#elif defined(__LDBL_MANT_DIG__)
    // If the compiler provides the number of mantissa digits, we can calculate the size in bytes
    #if __LDBL_MANT_DIG__ == 64
        #define NEX_SIZEOF_LONG_DOUBLE 12 // x86 80-bit extended
    #elif __LDBL_MANT_DIG__ == 113
        #define NEX_SIZEOF_LONG_DOUBLE 16 // 128-bit quad
    #else  // Fallback to standard double
        #define NEX_SIZEOF_LONG_DOUBLE 8
    #endif  // ^^__LDBL_MANT_DIG__
#else  // Last resort fallback
    #define NEX_SIZEOF_LONG_DOUBLE  sizeof(long double)
#endif  // ^^NEX_SIZEOF_LONG_DOUBLE

// ================================================================================
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
