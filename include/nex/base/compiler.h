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

#define NEX_COMPILER_IS_CLANG_BASED     0   // Compiler is Clang-based (LLVM or Apple Clang)
#define NEX_COMPILER_IS_GCC             0   // Compiler is GCC (GNU Compiler Collection)
#define NEX_COMPILER_IS_MSVC            0   // Compiler is MSVC (Microsoft Visual C++)

/**
 * Clang masquerades as GCC on POSIX and as MSVC on Windows, so we define compatibility macros to detect 
 * if we are using a compiler that is compatible with GCC or MSVC, which can be useful for conditional compilation 
 * of code that relies on specific compiler features or behaviors.
 */

#define NEX_COMPILER_GCC_COMPATIBLE     0   // Compiler is GCC-compatible (e.g., Clang on POSIX)
#define NEX_COMPILER_MSVC_COMPATIBLE    0   // Compiler is MSVC-compatible (e.g., Clang on Windows in MSVC compat. mode)

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
// Compiler detection
// ======================================================================================

// Detects the compiler being used to compile the code by checking for predefined macros 
// that are unique to each compiler.
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

// ================================================================================
// Compiler-specific preprocessor macros for feature detection
// ================================================================================

/**
 * @def NEX_HAS_INCLUDE(x)
 * @brief Checks if a header file is available for inclusion.
 *
 * @details
 * A standard C++17 preprocessor operator used to detect whether a specified header file
 * or source file (e.g., <optional>, "config.h") exists and can be included by the preprocessor.
 * This macro abstracts the underlying inclusion-checking mechanism to provide consistent availability
 * detection across supported compilers.
 * It evaluates to 1 if the header file is available, or 0 if it cannot be found.
 * Universally supported by all modern compilers (MSVC, GCC, Clang).
 */

// Define as 0 if the compiler does not support __has_include.
#if !defined(__has_include)
    #define __has_include(x) 0
#endif  // !defined(__has_include)

#if !defined(NEX_HAS_INCLUDE)
    #define NEX_HAS_INCLUDE(x) __has_include(x)
#endif  // !defined(NEX_HAS_INCLUDE)

/**
 * @def NEX_HAS_CPP_ATTRIBUTE(x)
 * @brief Checks if a C++ attribute is supported by the compiler.
 *
 * @details
 * A standard C++ feature-test operator used to detect whether the compiler supports
 * a specific C++ attribute (e.g., [[nodiscard]], [[fallthrough]]).
 * It returns a non-zero integer representing the version/year the attribute was introduced
 * if the attribute is supported, or 0 otherwise.
 * Supported across all major modern compilers (MSVC, GCC, Clang).
 *
 * @see https://wg21.link/sd6#testing-for-the-presence-of-an-attribute-__has_cpp_attribute
 * @see https://wg21.link/cpp.cond#:__has_cpp_attribute
 */

// Define as 0 if the compiler does not support __has_cpp_attribute.
#if !defined(__has_cpp_attribute)
    #define __has_cpp_attribute(x) 0
#endif  // !defined(__has_cpp_attribute)

#if !defined(NEX_HAS_CPP_ATTRIBUTE)
    #define NEX_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif  // !defined(NEX_HAS_CPP_ATTRIBUTE)

/**
 * @def NEX_HAS_BUILTIN(x)
 * @brief Checks if a compiler builtin is available.
 *
 * @details
 * A compiler-specific feature-test operator primarily used in Clang and GCC to determine if
 * a specific compiler builtin function (e.g., __builtin_trap, __builtin_popcount) is available.
 * It evaluates to 1 if the builtin is supported, or 0 otherwise.
 *
 * @note Not natively supported by MSVC.
 */

// Define as 0 if the compiler does not support __has_builtin.
#if !defined(__has_builtin)
    #define __has_builtin(x) 0
#endif  // !defined(__has_builtin)

#if !defined(NEX_HAS_BUILTIN)
    #define NEX_HAS_BUILTIN(x) __has_builtin(x)
#endif  // !defined(NEX_HAS_BUILTIN)

/**
 * @def NEX_HAS_CONSTEXPR_BUILTIN(x)
 * @brief Checks if a compiler constexpr builtin is available.
 *
 * @details
 * A compiler-specific feature-test operator primarily used in Clang and GCC to determine if
 * a specific compiler constexpr builtin function (e.g., __builtin_constant_p) is available.
 * It evaluates to 1 if the builtin is supported, or 0 otherwise.
 *
 * @note Not natively supported by MSVC.
 */

// Define as 0 if the compiler does not support __has_constexpr_builtin.
#if !defined(__has_constexpr_builtin)
    #define __has_constexpr_builtin(x) 0
#endif  // !defined(__has_constexpr_builtin)

#if !defined(NEX_HAS_CONSTEXPR_BUILTIN)
    #define NEX_HAS_CONSTEXPR_BUILTIN(x) __has_constexpr_builtin(x)
#endif  // !defined(NEX_HAS_CONSTEXPR_BUILTIN)

/**
 * @def NEX_HAS_ATTRIBUTE(x)
 * @brief Checks if a compiler attribute is supported by the compiler.
 *
 * @details
 * A GNU/Clang extension operator used to check for the availability of compiler-specific attributes
 * applied via the `__attribute__((...))` syntax (e.g., `__attribute__((always_inline))`).
 * It evaluates to 1 if the attribute is supported, or 0 otherwise. 
 *
 * @note Not natively supported by MSVC.
 */

// Define as 0 if the compiler does not support __has_attribute.
#if !defined(__has_attribute)
    #define __has_attribute(x) 0
#endif  // !defined(__has_attribute)

#if !defined(NEX_HAS_ATTRIBUTE)
    #define NEX_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif  // !defined(NEX_HAS_ATTRIBUTE)

/**
 * @def NEX_HAS_FEATURE(x)
 * @brief Checks if a compiler feature is available.
 *
 * @details
 * A proprietary Clang feature-test operator (also supported by GCC) used to check if a specific language feature
 * (e.g., cxx_rvalue_references, address_sanitizer) is strictly enabled under the current language standard
 * and compilation flags.
 * It evaluates to 1 if the feature is enabled, or 0 otherwise. 
 *
 * @note Not supported by MSVC.
 */

// Define as 0 if the compiler does not support __has_feature.
#if !defined(__has_feature)
    #define __has_feature(x) 0
#endif  // !defined(__has_feature)

#if !defined(NEX_HAS_FEATURE)
    #define NEX_HAS_FEATURE(x) __has_feature(x)
#endif  // !defined(NEX_HAS_FEATURE)

/**
 * @def NEX_HAS_EXTENSION(x)
 * @brief Checks if a compiler extension is available.
 *
 * @details
 * A proprietary Clang/GCC feature-test operator used to check if a language feature is supported by the compiler,
 * either as part of the active language standard or as a backwards-compatible compiler extension.
 * It evaluates to 1 if the extension is available, or 0 otherwise.
 *
 * @note Not natively supported by MSVC.
 */

// Define as 0 if the compiler does not support __has_extension.
#if !defined(__has_extension)
    #define __has_extension(x) 0
#endif  // !defined(__has_extension)

#if !defined(NEX_HAS_EXTENSION)
    #define NEX_HAS_EXTENSION(x) __has_extension(x)
#endif  // !defined(NEX_HAS_EXTENSION)

/**
 * @def NEX_HAS_KEYWORD
 * @brief Checks if a compiler keyword is available.
 *
 * @details
 * A proprietary Clang/GCC feature-test operator used to check if a specific compiler keyword
 * (e.g., __has_cpp_attribute, __has_builtin) is available in the current compilation context.
 * It evaluates to 1 if the keyword is available, or 0 otherwise.
 *
 * @note Not natively supported by MSVC.
 */

// '__is_identifier' returns '0' if '__x' is a reserved identifier provided by the compiler
// and '1' otherwise.
#if !defined(__is_identifier)
    #define __is_identifier(__x) 1
#endif  // !defined(__is_identifier)

#if !defined(__has_keyword)
    #define __has_keyword(__x) !(__is_identifier(__x))
#endif  // !defined(__has_keyword)

#if !defined(NEX_HAS_KEYWORD)
    #define NEX_HAS_KEYWORD(x) __has_keyword(x)
#endif  // !defined(NEX_HAS_KEYWORD)

// =================================================================================
// Language/compiler-specific type definitions and feature detection
// =================================================================================

#if NEX_HAS_CXX20 && defined(__cpp_char8_t)
    #define NEX_HAS_BUILTIN_CHAR8_T 1
#else  // Compiler does not support char8_t
    #define NEX_HAS_BUILTIN_CHAR8_T 0
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

#if defined(__SIZEOF_INT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_INT128 1
#else // Compiler does not support __int128
    #define NEX_HAS_BUILTIN_INT128 0
#endif  // ^^NEX_HAS_BUILTIN_INT128

#if defined(__FLT16_DIG__) || defined(__fp16) || defined(_Float16)
    #define NEX_HAS_BUILTIN_FLOAT16 1
    #if defined(__fp16) && (NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG_BASED)
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

// ================================================================================
// Compiler endian detection macros (on GCC/Clang)
// ================================================================================

#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
    #if !defined(__BYTE_ORDER__)
        #error Compiler does not define __BYTE_ORDER__ macro for endian detection.
    #endif
#endif  // ^^NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 1
        #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 0
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 0
        #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 1
    #else
        #error Unknown byte order detected by the compiler.
    #endif
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
    #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 1
    #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 0
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
    #define NEX_COMPILER_HAS_LITTLE_ENDIAN_MACRO 0
    #define NEX_COMPILER_HAS_BIG_ENDIAN_MACRO 1
#endif  // ^^Endianness detection
