/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    compiler_features.h
 * @brief   Provides compiler feature detection and syntax abstractions.
 *
 * @details
 * This header provides a consistent interface for detecting compiler features, extensions, builtins,
 * attributes, and header availability.
 *
 * It also defines compiler-specific syntax abstractions that allow compiler extensions to be expressed
 * consistently across supported compiler environments.
 *
 * The provided macros allow other configuration headers and source files to query compiler capabilities
 * and conditionally use compiler-specific functionality without directly depending on individual compiler
 * implementations.
 *
 * @note
 * This header provides feature detection and low-level compiler syntax abstractions.
 * It does not define NexCore's higher-level annotation API.
 *
 * @see attributes.h
 * @see visibility.h
 */

#include "nex/base/compiler.h"
#include "nex/base/abi.h"

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

/**
 * @def NEX_HAS_DECLSPEC_ATTRIBUTE
 * @brief Checks if a declspec attribute is available.
 *
 * @details
 * This macro uses the compiler-specific `__has_declspec_attribute` feature-test operator
 * to determine if a particular declspec attribute is supported.
 * It evaluates to 1 if the attribute is available, or 0 otherwise.
 */

#if !defined(__has_declspec_attribute)
    #define __has_declspec_attribute(x) 0
#endif  // !defined(__has_declspec_attribute)

#if !defined(NEX_HAS_DECLSPEC_ATTRIBUTE)
    #define NEX_HAS_DECLSPEC_ATTRIBUTE(x) __has_declspec_attribute(x)
#endif  // !defined(NEX_HAS_DECLSPEC_ATTRIBUTE)

// =================================================================================
// Compiler-specific attribute detection and annotation macros
// =================================================================================

/**
 * @def NEX_HAS_CLANG_ATTRIBUTE(x)
 * @brief Checks if a Clang-specific attribute is supported by the compiler.
 * 
 * @details
 * This macro checks for the presence of a Clang-specific attribute using `__has_cpp_attribute`.
 * It evaluates to 1 if the attribute is supported and the compiler is Clang, otherwise it evaluates to 0.
 */
#if NEX_COMPILER_IS_CLANG_BASED
    #define NEX_HAS_CLANG_ATTRIBUTE(x) NEX_HAS_CPP_ATTRIBUTE(clang::x)
#else
    #define NEX_HAS_CLANG_ATTRIBUTE(x) 0
#endif  // ^^NEX_HAS_CLANG_ATTRIBUTE

/**
 * @def NEX_CLANG_ATTRIBUTE(x)
 * @brief Annotates code with a Clang-specific attribute.
 * 
 * @details
 * This macro allows you to annotate code with a Clang-specific attribute.
 * It expands to the appropriate Clang attribute syntax if the compiler is Clang,
 * and expands to nothing on other compilers.
 */
#if NEX_COMPILER_IS_CLANG_BASED
    #define NEX_CLANG_ATTRIBUTE(x) [[clang::x]]
#else
    #define NEX_CLANG_ATTRIBUTE(x)
#endif  // ^^NEX_CLANG_ATTRIBUTE

/**
 * @def NEX_HAS_GCC_ATTRIBUTE(x)
 * @brief Checks if a GCC-specific attribute is supported by the compiler.
 * 
 * @details
 * This macro checks for the presence of a GCC-specific attribute using `__has_cpp_attribute`.
 * It evaluates to 1 if the attribute is supported and the compiler is GCC-compatible, and 0 otherwise.
 */
#if NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_HAS_GCC_ATTRIBUTE(x) NEX_HAS_CPP_ATTRIBUTE(gnu::x)
#else
    #define NEX_HAS_GCC_ATTRIBUTE(x) 0
#endif  // ^^NEX_HAS_GCC_ATTRIBUTE

/**
 * @def NEX_GCC_ATTRIBUTE(x)
 * @brief Annotates code with a GCC-specific attribute.
 * 
 * @details
 * This macro allows you to annotate code with a GCC-specific attribute.
 * It expands to the appropriate attribute syntax if the compiler is GCC-compatible,
 * and expands to nothing on other compilers.
 */
#if NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_GCC_ATTRIBUTE(x) [[gnu::x]]
#else
    #define NEX_GCC_ATTRIBUTE(x)
#endif  // ^^NEX_GCC_ATTRIBUTE

/**
 * @def NEX_GCC_OR_CLANG_ATTRIBUTE(x)
 * @brief Annotates code with a GCC or Clang-specific attribute using older syntax.
 * 
 * @details
 * This macro allows you to annotate code with a GCC or Clang-specific attribute using
 * the older `__attribute__((x))` syntax.
 * It expands to the appropriate attribute syntax if the compiler is GCC-compatible or Clang,
 * and expands to nothing on other compilers.
 */
#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_GCC_OR_CLANG_ATTRIBUTE(x) __attribute__((x))
#else
    #define NEX_GCC_OR_CLANG_ATTRIBUTE(x)
#endif  // ^^NEX_GCC_OR_CLANG_ATTRIBUTE

/**
 * @def NEX_HAS_MSVC_ATTRIBUTE(x)
 * @brief Checks if a Microsoft-specific attribute is supported by the compiler.
 * 
 * @details
 * This macro checks for the presence of a Microsoft-specific attribute using `__has_cpp_attribute`.
 * It evaluates to 1 if the attribute is supported and the compiler is MSVC-compatible, and 0 otherwise.
 */
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_MSVC_ATTRIBUTE(x) NEX_HAS_CPP_ATTRIBUTE(msvc::x)
#else
    #define NEX_HAS_MSVC_ATTRIBUTE(x) 0
#endif  // ^^NEX_HAS_MSVC_ATTRIBUTE

/**
 * @def NEX_MSVC_ATTRIBUTE(x)
 * @brief Annotates code with a Microsoft-specific attribute.
 * 
 * @details
 * This macro allows you to annotate code with a Microsoft-specific attribute.
 * It expands to the appropriate attribute syntax if the compiler is MSVC-compatible,
 * and expands to nothing on other compilers.
 */
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_MSVC_ATTRIBUTE(x) [[msvc::x]]
#else
    #define NEX_MSVC_ATTRIBUTE(x)
#endif  // ^^NEX_MSVC_ATTRIBUTE

/**
 * @def NEX_MSVC_DECLSPEC
 * @brief Annotates a symbol with a Microsoft-specific __declspec attribute.
 * 
 * @details
 * This macro allows you to annotate a symbol with a Microsoft-specific __declspec attribute.
 * It expands to the appropriate attribute syntax if the compiler is MSVC-compatible,
 * and expands to nothing on other compilers.
 */
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_MSVC_DECLSPEC(x) __declspec(x)
#else
    #define NEX_MSVC_DECLSPEC(x)
#endif  // ^^NEX_MSVC_DECLSPEC

// =================================================================================
// Language/compiler-specific type definitions and feature detection
// =================================================================================

// If we are getting operator new from the MSVC CRT, then allocation overloads
// for align_val type were added in MSVC v19.12, a.k.a Visual Studio 2017 version 15.3.
#if defined(NEX_MSVCRT) && NEX_COMPILER_MSVC_COMPATIBLE && NEX_COMPILER_MSVC_VER < 1912
    #define NEX_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#elif defined(NEX_ABI_VCRUNTIME) && !defined(__cpp_aligned_new)
    // We're deferring to Microsoft's STL to provide aligned new et al.
    // We don't have it unless the language feature test macro is defined.
    #define NEX_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#elif defined(__MVS__)
    #define NEX_HAS_LIBRARY_ALIGNED_ALLOCATION 0
#else
    #define NEX_HAS_LIBRARY_ALIGNED_ALLOCATION 1
#endif  // ^^NEX_HAS_LIBRARY_ALIGNED_ALLOCATION

#if !NEX_HAS_LIBRARY_ALIGNED_ALLOCATION
    #define NEX_HAS_ALIGNED_ALLOCATION 0
#elif defined(__cpp_aligned_new) /* && __cpp_aligned_new >= 201606 */
    #define NEX_HAS_ALIGNED_ALLOCATION 1
#else  // Compiler does not support aligned allocation
    #define NEX_HAS_ALIGNED_ALLOCATION 0
#endif  // ^^NEX_HAS_ALIGNED_ALLOCATION

#if defined(__cpp_sized_deallocation) /* && __cpp_sized_deallocation >= 201309L */
    #define NEX_HAS_SIZED_DEALLOCATION 1
    #define NEX_ONLY_IF_SIZED_DEALLOCATION(...) __VA_OPT__(, ) __VA_ARGS__
#else
    #define NEX_HAS_SIZED_DEALLOCATION 0
    #define NEX_ONLY_IF_SIZED_DEALLOCATION(...) /* expand to nothing */
#endif  // ^^NEX_HAS_SIZED_DEALLOCATION

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
        using _nex_float16_t = __fp16;
    #else  // _Float16 
        using _nex_float16_t = _Float16;
    #endif
#else  // Compiler does not support 16-bit floating-point
    #define NEX_HAS_BUILTIN_FLOAT16 0
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else  // Compiler does not support __float128
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

#if defined(__CHAR_BIT__)
    #define NEX_COMPILER_PROVIDED_CHAR_BIT __CHAR_BIT__
#endif  // ^^NEX_COMPILER_PROVIDED_CHAR_BIT
