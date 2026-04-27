/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      attributes.h
 * @brief     Defines macros for common attributes and annotations used in the codebase, such as deprecation, nodiscard, 
 *            noinline, and noreturn.
 * 
 * @details
 * This section defines macros for marking functions and classes with attributes such as export/import (for shared libraries), 
 * deprecation (with or without a message), nodiscard (to indicate that the return value should not be ignored), noinline 
 * (to indicate that a function should not be inlined), and noreturn (to indicate that a function does not return).
 * The macros are defined based on the compiler being used, and they can be used throughout the codebase to provide 
 * consistent annotations and attributes for functions and classes.
 * 
 * @note When using the NEX_DEPRECATED macro, it is recommended to provide a message that explains why 
 *       the function or class is deprecated and what should be used instead.
 */

#include "nex/base/compiler.h"

/**
 * @section Attribute and Annotation Macros
 * @brief   Defines macros for common attributes and annotations.
 */

/**
 * @def NEX_HAS_CPP_ATTRIBUTE(x)
 * @brief Check if a C++ attribute is supported by the compiler
 * 
 * @details
 * This is a wrapper around `__has_cpp_attribute`, which can be used to test for the presence of an attribute. 
 * In case the compiler does not support this macro it will simply evaluate to 0.
 *
 * References:
 * @see https://wg21.link/sd6#testing-for-the-presence-of-an-attribute-__has_cpp_attribute
 * @see https://wg21.link/cpp.cond#:__has_cpp_attribute
 */
#if defined(__has_cpp_attribute)
    #define NEX_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define NEX_HAS_CPP_ATTRIBUTE(x) 0
#endif

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
#else
    #define NEX_HAS_BUILTIN(x) 0
#endif

// __has_feature and __has_attribute are not available in MSVC.
#if !defined(__has_feature)
    #define __has_feature(x) 0
#endif  // !defined(__has_feature)

#if !defined(__has_attribute)
    #define __has_attribute(x) 0
#endif  // !defined(__has_attribute)

/**
 * @def NEX_HAS_BUILTIN_FLOAT128
 * @brief This macro indicates whether the compiler supports the built-in `__float128` type
 *       (available in GCC and Clang on platforms with 128-bit floating-point support).
 * 
 * @details
 * The `__float128` type provides extended precision floating-point arithmetic, which can be useful for certain 
 * numerical applications. This macro can be used to conditionally compile code that relies on `__float128` when it is 
 * available, while providing fallback implementations or alternative code paths when it is not supported by the compiler.
 */
#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_IS_MSVC
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif

/**
 * @def NEX_NOINLINE
 * @brief Annotate a function indicating it should not be inlined.
 * 
 * @details
 * This macro can be used to prevent the compiler from inlining a function, which can be useful for debugging,
 * reducing code size, or ensuring that a function call boundary is preserved for performance analysis.
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler.
 * For example, in GCC it expands to `__attribute__((noinline))`, while in MSVC it expands to `__declspec(noinline)`.
 * If the compiler does not support a noinline attribute, the macro expands to nothing, allowing the code to compile 
 * without errors.
 * 
 * Example usage:
 * @code
 *   NEX_NOINLINE void DoStuff() { ... }
 * @endcode
 */
#if NEX_COMPILER_IS_GCC
    #define NEX_NOINLINE __attribute__((noinline))
#elif NEX_COMPILER_IS_MSVC
    #define NEX_NOINLINE __declspec(noinline)
#else
    #define NEX_NOINLINE
#endif

/**
 * @def NEX_ALWAYS_INLINE
 * @brief Annotate a function indicating it should always be inlined.
 * 
 * @details
 * This macro can be used to suggest to the compiler that a function should always be inlined, which can improve 
 * performance by eliminating function call overhead and enabling further compiler optimizations. 
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler.
 * 
 * Example usage:
 * @code
 *   NEX_ALWAYS_INLINE void FastFunction() { ... }
 * @endcode
 */
#if NEX_COMPILER_IS_GCC && defined(NDEBUG)
    #define NEX_ALWAYS_INLINE inline __attribute__((__always_inline__))
#elif NEX_COMPILER_IS_MSVC && defined(NDEBUG)
    #define NEX_ALWAYS_INLINE __forceinline
#else
    #define NEX_ALWAYS_INLINE inline
#endif

/**
 * @def NEX_NODISCARD
 * @brief Mark functions whose return values should not be discarded
 * 
 * @details
 * Expands to [[nodiscard]] when C++17 or later is available, otherwise empty.
 * This attribute encourages callers to use function return values.
 */
#ifndef NEX_NODISCARD
    #ifndef NEX_HAS_NODISCARD
        #ifndef __has_cpp_attribute
            #define NEX_HAS_NODISCARD 0
        #elif __has_cpp_attribute(nodiscard) >= 201603L // TRANSITION, VSO#939899 (need toolset update)
            #define NEX_HAS_NODISCARD 1
        #else
            #define NEX_HAS_NODISCARD 0
        #endif
    #endif // !defined NEX_HAS_NODISCARD
    #if NEX_HAS_NODISCARD
        #define NEX_NODISCARD [[nodiscard]]
    #else // ^^^ CAN HAZ [[nodiscard]] / NO CAN HAZ [[nodiscard]] vvv
        #define NEX_NODISCARD
    #endif // NEX_HAS_NODISCARD
#endif // !defined NEX_NODISCARD

/**
 * @def NEX_ALIGNAS(byte_alignment)
 * @brief Specify memory alignment for types and variables
 * 
 * @details
 * This macro specifies the memory alignment for structs, classes, and variables.
 * While C++11's `alignas` keyword is preferred in most cases, this macro is needed
 * in situations where compiler-specific attributes must be mixed with alignment.
 * 
 * @param byte_alignment The desired alignment in bytes (must be a power of 2)
 * 
 * @par Usage Examples:
 * @code
 * class NEX_ALIGNAS(16) MyClass { ... };  // Align class to 16 bytes
 * NEX_ALIGNAS(16) int array[4];           // Align array to 16 bytes
 * @endcode
 * 
 * @par Compiler Compatibility:
 * The C++11 `alignas` keyword cannot always be mixed with compiler attributes:
 * - Does NOT work: `struct alignas(16) __attribute__((packed)) S { char c; };`
 * - Works in Clang only: `struct __attribute__((packed)) alignas(16) S { char c; };`
 * - Works in both: `struct alignas(16) S { char c; } __attribute__((packed));`
 * 
 * @note Some attributes (like visibility for exported classes) must be specified
 *       before the class definition, making `alignas` unusable in those cases.
 */
#if NEX_COMPILER_IS_MSVC
    #define NEX_ALIGNAS(byte_alignment) __declspec(align(byte_alignment))
#elif NEX_COMPILER_IS_GCC
    #define NEX_ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#endif

/**
 * @def NEX_CONSTEXPR20
 * @brief Expands to `constexpr` when compiling with C++20 support, otherwise empty
 * 
 * @details
 * Use this to annotate functions that can be `constexpr` in C++20 but not in C++17.
 * This allows the same codebase to take advantage of C++20 features when available without breaking 
 * compatibility with C++17.
 * 
 * @note 
 * Some functions can only be marked as `constexpr` in C++20 but not in C++17 due to relaxed rules 
 * around what is allowed in `constexpr` functions. For example, C++20 allows for more complex control 
 * flow and certain standard library functions to be used in `constexpr` contexts. For functions that 
 * can be marked as `constexpr` in both C++17 and C++20 without causing any compilation issues, simply 
 * use `constexpr` directly without this macro.
 * 
 * Example usage:
 * @code
 * NEX_CONSTEXPR20 int Add(int a, int b) {
 *     return a + b;
 * }
 * @endcode
 */
#if NEX_HAS_CXX20
    #define NEX_CONSTEXPR20 constexpr
#else
    #define NEX_CONSTEXPR20
#endif

/**
 * @def NEX_NO_UNIQUE_ADDRESS
 * @brief Allow data members to share addresses (C++20)
 * 
 * @details
 * When supported by the compiler, this expands to the C++20 attribute
 * [[no_unique_address]]. This allows annotating data members so they need not
 * have an address distinct from all other non-static data members of the class,
 * enabling empty base optimization for members.
 * 
 * @par Usage Example:
 * @code
 * struct Empty {};
 * struct MyStruct {
 *     NEX_NO_UNIQUE_ADDRESS Empty e;  // May have zero size
 *     int value;
 * };
 * @endcode
 * 
 * @see https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
 * @see https://wg21.link/dcl.attr.nouniqueaddr
 */
#if NEX_HAS_CPP_ATTRIBUTE(no_unique_address)
    #define NEX_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define NEX_NO_UNIQUE_ADDRESS
#endif

/**
 * @def NEX_NORETURN
 * @brief Mark a function as not returning (i.e., it will not return to the caller)
 * 
 * @details
 * This macro can be used to indicate that a function does not return to the caller, which can help 
 * the compiler optimize code and generate better warnings. For example, functions that throw exceptions, 
 * call `std::exit()`, or enter an infinite loop can be marked with this attribute. The macro expands 
 * to the appropriate compiler-specific attribute based on the detected compiler. If the compiler does not 
 * support a noreturn attribute, the macro expands to nothing, allowing the code to compile without errors.
 */
#if NEX_COMPILER_IS_MSVC
    #define NEX_NORETURN __declspec(noreturn)
#elif NEX_COMPILER_IS_GCC
    #define NEX_NORETURN __attribute__((noreturn))
#else
    #define NEX_NORETURN
#endif

/**
 * @def NEX_DEPRECATED(msg)
 * @brief Mark a function or class as deprecated with a custom message
 * 
 * @details
 * This macro can be used to indicate that a function or class is deprecated, meaning it should not be used 
 * in new code and may be removed in future versions. 
 * The `msg` parameter allows you to provide a custom message that explains why the function or class is deprecated 
 * and what should be used instead. 
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler. If the compiler 
 * does not support a deprecation attribute, the macro expands to nothing, allowing the code to compile without errors.
 * 
 * Example usage:
 * @code
 *   NEX_DEPRECATED("Use NewFunction() instead") void OldFunction() { ... }
 * @endcode
 */
#if NEX_COMPILER_IS_MSVC
    #define NEX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif NEX_COMPILER_IS_GCC
    #define NEX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
    #define NEX_DEPRECATED(msg)
#endif

// A version of NEX_DEPRECATED that does not require a message (for compilers that support it)
#define NEX_DEPRECATED_NO_MSG() NEX_DEPRECATED("")

// Mark a function as no-opt
#define NEX_NO_OPT {/* no-opt */}

// Mark a parameter as maybe unused to avoid compiler warnings
#define NEX_UNUSED_PARAM(param) (void)(param)

// Mark a variable as maybe unused to avoid compiler warnings
#define NEX_UNUSED_VAR(var) (void)(var)

// Mark multiple variables or parameters as maybe unused to avoid compiler warnings
#define NEX_UNUSED(...) \
    (void)(sizeof((int[]){(NEX_UNUSED_VAR(__VA_ARGS__), 0)...}));

// Mark a function as pure virtual (i.e., it must be overridden by derived classes)
#define NEX_PURE_VIRTUAL(func) \
    virtual func = 0

// Mark a function as a no-opt virtual (i.e., it must be overridden by derived classes, and the base implementation does nothing)
#define NEX_NO_OPT_VIRTUAL(func) \
    virtual func { NEX_NO_OPT; }
