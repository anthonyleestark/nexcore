/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      attributes.h
 * @brief     Defines macros for common attributes and annotations used in the codebase, such as deprecation, 
 *            nodiscard, noinline, and noreturn.
 * 
 * @details
 * This section defines macros for marking functions and classes with attributes such as export/import 
 * (for shared libraries), deprecation (with or without a message), nodiscard (to indicate that the return value 
 * should not be ignored), noinline (to indicate that a function should not be inlined), and noreturn (to indicate 
 * that a function does not return).
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

/**
 * @def NEX_HAS_BUILTIN_FLOAT128
 * @brief This macro indicates whether the compiler supports the built-in `__float128` type
 *       (available in GCC and Clang on platforms with 128-bit floating-point support).
 * 
 * @details
 * The `__float128` type provides extended precision floating-point arithmetic, which can be useful for certain 
 * numerical applications. This macro can be used to conditionally compile code that relies on `__float128` when 
 * it is available, while providing fallback implementations or alternative code paths when it is not supported 
 * by the compiler.
 */
#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else  // Compiler does not support __float128
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif  // defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_MSVC_COMPATIBLE

/**
 * @def NEX_NOINLINE
 * @brief Annotate a function indicating it should not be inlined.
 * 
 * @details
 * This macro can be used to prevent the compiler from inlining a function, which can be useful for debugging,
 * reducing code size, or ensuring that a function call boundary is preserved for performance analysis.
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler.
 * For example, it expands to `[[clang::noinline]]` for Clang, `[[gnu::noinline]]` for GCC, and `[[msvc::noinline]]` 
 * for MSVC. If the compiler does not support a noinline attribute, it expands to nothing.
 * 
 * @par See also:
 *      https://clang.llvm.org/docs/AttributeReference.html#noinline
 * 
 * Example usage:
 * @code
 *   NEX_NOINLINE void DoStuff() { 
 *       // This body will not be inlined into callers. 
 *   }
 * @endcode
 */
#if NEX_HAS_CPP_ATTRIBUTE(clang::noinline)
    #define NEX_NOINLINE [[clang::noinline]]
#elif NEX_HAS_CPP_ATTRIBUTE(gnu::noinline)
    #define NEX_NOINLINE [[gnu::noinline]]
#elif NEX_HAS_CPP_ATTRIBUTE(msvc::noinline)
    #define NEX_NOINLINE [[msvc::noinline]]
#else  // Compiler does not support [[noinline]] attribute
    #define NEX_NOINLINE
#endif  // NEX_NOINLINE

/**
 * @def NEX_ALWAYS_INLINE
 * @brief Annotate a function indicating it should always be inlined.
 * 
 * @details
 * This macro can be used to suggest to the compiler that a function should always be inlined, which can 
 * improve performance by eliminating function call overhead and enabling further compiler optimizations. 
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler.
 * 
 * @note Since `NEX_ALWAYS_INLINE` is performance-oriented but can hamper debugging, ignore it in debug mode.
 * 
 * @par See also:
 *      https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline
 * 
 * Example usage:
 * @code
 *   NEX_ALWAYS_INLINE void FastFunction() { 
 *       // This body will be inlined into callers whenever possible. 
 *   }
 * @endcode
 */
#if defined(NDEBUG)
    #if NEX_HAS_CPP_ATTRIBUTE(clang::always_inline)
        #define NEX_ALWAYS_INLINE [[clang::always_inline]] inline
    #elif NEX_HAS_CPP_ATTRIBUTE(gnu::always_inline)
        #define NEX_ALWAYS_INLINE [[gnu::always_inline]] inline
    #elif NEX_COMPILER_MSVC_COMPATIBLE
        #define NEX_ALWAYS_INLINE __forceinline
    #endif
#endif

#if !defined(NEX_ALWAYS_INLINE)
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
#ifndef NEX_HAS_NODISCARD
    #if NEX_HAS_CPP_ATTRIBUTE(nodiscard) >= NEX_CXX17_VER_NUMBER
        #define NEX_HAS_NODISCARD 1
    #else  // Compiler does not support [[nodiscard]] or we are in C++14 or earlier
        #define NEX_HAS_NODISCARD 0
    #endif  // NEX_HAS_NODISCARD
#endif  // !defined(NEX_HAS_NODISCARD)

#if NEX_HAS_NODISCARD
    #define NEX_NODISCARD [[nodiscard]]
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_NODISCARD __attribute__((warn_unused_result))
#elif NEX_COMPILER_MSVC_COMPATIBLE && NEX_COMPILER_MSVC >= 1700
    #define NEX_NODISCARD _Check_return_
#endif  // NEX_HAS_NODISCARD

#if !defined(NEX_NODISCARD)
    #define NEX_NODISCARD   // No-op if NODISCARD is not supported
#endif  // !defined(NEX_NODISCARD)

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
#if NEX_CXX_VER >= NEX_CXX11_VER_NUMBER
    #define NEX_ALIGNAS(byte_alignment) alignas(byte_alignment)
#elif NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_ALIGNAS(byte_alignment) __declspec(align(byte_alignment))
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#endif  // NEX_ALIGNAS

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
#else  // Compiler does not support C++20 constexpr
    #define NEX_CONSTEXPR20
#endif  // NEX_CONSTEXPR20

/**
 * @def NEX_NO_UNIQUE_ADDRESS
 * @brief Allow data members to share addresses (C++20)
 * 
 * @details
 * When supported by the compiler, this expands to the C++20 attribute [[no_unique_address]]. 
 * This allows annotating data members so they need not have an address distinct from all other 
 * non-static data members of the class, enabling empty base optimization for members.
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
#if NEX_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
    #define NEX_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif NEX_HAS_CPP_ATTRIBUTE(no_unique_address)
    #define NEX_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else  // Compiler does not support [[no_unique_address]]
    #define NEX_NO_UNIQUE_ADDRESS
#endif  // NEX_NO_UNIQUE_ADDRESS

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
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_NORETURN __declspec(noreturn)
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_NORETURN __attribute__((noreturn))
#else  // Compiler does not support noreturn attribute
    #define NEX_NORETURN
#endif  // NEX_NORETURN

/**
 * @def NEX_DEPRECATED(msg)
 * @brief Mark a function or class as deprecated with a custom message
 * 
 * @details
 * This macro can be used to indicate that a function or class is deprecated, meaning it should not be used 
 * in new code and may be removed in future versions. 
 * The `msg` parameter allows you to provide a custom message that explains why the function or class is 
 * deprecated and what should be used instead. 
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler. If the 
 * compiler does not support a deprecation attribute, the macro expands to nothing, allowing the code to 
 * compile without errors.
 * 
 * Example usage:
 * @code
 *   NEX_DEPRECATED("Use NewFunction() instead") void OldFunction() { ... }
 * @endcode
 */
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else  // Compiler does not support deprecation attribute
    #define NEX_DEPRECATED(msg)
#endif  // NEX_DEPRECATED

// A version of NEX_DEPRECATED that does not require a message (for compilers that support it)
#if NEX_HAS_CPP_ATTRIBUTE(deprecated) >= NEX_CXX14_VER_NUMBER
    #define NEX_DEPRECATED_NO_MSG() [[deprecated]]
#else  // Compiler does not support [[deprecated]] without a message, or we are in C++11 or earlier
    #define NEX_DEPRECATED_NO_MSG() NEX_DEPRECATED("")
#endif  // NEX_DEPRECATED_NO_MSG

/**
 * @def NEX_MAYBE_UNUSED
 * @brief Mark a function, variable, or parameter as maybe unused to suppress compiler warnings
 * 
 * @details
 * This macro can be used to indicate that a function, variable, or parameter may be intentionally unused, 
 * which can help suppress compiler warnings about unused code. The macro expands to the appropriate 
 * compiler-specific attribute based on the detected compiler. If the compiler does not support a maybe_unused 
 * attribute, the macro expands to nothing, allowing the code to compile without errors.
 * 
 * Example usage:
 * @code
 *   NEX_MAYBE_UNUSED void HelperFunction() { ... }
 *   void MainFunction(int param) {
 *       NEX_MAYBE_UNUSED int unusedVar = 42;
 *       // ...
 *   }
 * @endcode
 */
#if NEX_HAS_CPP_ATTRIBUTE(maybe_unused) >= NEX_CXX17_VER_NUMBER
    #define NEX_MAYBE_UNUSED [[maybe_unused]]
#else  // Compiler does not support [[maybe_unused]] or we are in C++14 or earlier
    #define NEX_MAYBE_UNUSED
#endif  // NEX_MAYBE_UNUSED

// Mark a parameter as maybe unused to avoid compiler warnings
#define NEX_UNUSED_PARAM(param) (void)(param)

// Mark a variable as maybe unused to avoid compiler warnings
#define NEX_UNUSED_VAR(var) (void)(var)

/** 
 * @def NEX_UNUSED
 * @brief Mark multiple variables or parameters as maybe unused to avoid compiler warnings
 * 
 * @details
 * This macro provides a unified, cross-platform way to suppress "unused variable" warnings. 
 * It adapts its implementation based on the available C++ standard to ensure maximum efficiency 
 * and compatibility.
 * - In C++17 and later: It utilizes a variadic template function (@ref NEX_UNUSED_FUNC). 
 *   By passing the variables as forwarding references to an inline empty function, the compiler marks 
 *   them as "accessed." Modern compilers will optimize this call away entirely, resulting in zero 
 *   runtime overhead.
 * - Pre-C++17: It employs a compile-time "sizeof" trick. It creates a zero-cost temporary array 
 *   (compound literal) where each element is initialized by the variable being suppressed. 
 *   Since sizeof is evaluated during compilation, no machine code is generated for the array at runtime.
 * 
 * Example usage:
 * @code
 *   void FunctionWithUnusedParams(int usedParam, int unusedParam1, int unusedParam2) {
 *       NEX_UNUSED(unusedParam1, unusedParam2);
 *       // Function logic using usedParam...
 *   }
 * @endcode
 */
#if NEX_HAS_CXX17
    template <typename... Args>
    inline void NEX_UNUSED_FUNC(Args&&... args) {}
    #define NEX_UNUSED(...) NEX_UNUSED_FUNC(__VA_ARGS__)
#else  // C++14 or earlier
    #define NEX_UNUSED(...) \
        (void)(sizeof((int[]){(NEX_UNUSED_VAR(__VA_ARGS__), 0)...}));
#endif

/**
 * @def NEX_UNLIKELY(x)
 * @brief Hint that an expression is likely to be false
 * 
 * @details
 * Provides a branch prediction hint to the compiler that the expression is unlikely to be true, 
 * which can improve performance through better code layout.
 * 
 * @param x The expression to evaluate
 * @return The boolean result of the expression
 */
#if !defined(NEX_UNLIKELY)
    #if NEX_HAS_CXX20
        #define NEX_UNLIKELY(x) [[unlikely]] (x)
    #elif NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
        #define NEX_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else  // Compiler does not support __builtin_expect
        #define NEX_UNLIKELY(x) (x)
    #endif  // NEX_HAS_CXX20
#endif  // !defined(NEX_UNLIKELY)

/**
 * @def NEX_LIKELY(x)
 * @brief Hint that an expression is likely to be true
 * 
 * @details
 * Provides a branch prediction hint to the compiler that the expression is likely to be true, 
 * which can improve performance through better code layout.
 * 
 * @param x The expression to evaluate
 * @return The boolean result of the expression
 */
#if !defined(NEX_LIKELY)
    #if NEX_HAS_CXX20
        #define NEX_LIKELY(x) [[likely]] (x)
    #elif NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
        #define NEX_LIKELY(x) __builtin_expect(!!(x), 1)
    #else  // Compiler does not support __builtin_expect
        #define NEX_LIKELY(x) (x)
    #endif  // NEX_HAS_CXX20
#endif  // !defined(NEX_LIKELY)

/**
 * @def NEX_NOMERGE
 * @brief Annotates a function, function pointer, or statement to disallow optimizations that merge calls. 
 *        Useful to ensure the source locations of such calls are not obscured.
 *
 * @par See also:
 *      https://clang.llvm.org/docs/AttributeReference.html#nomerge
 *
 * Usage:
 * @code
 *   NEX_NOMERGE void Func();  // No direct calls to `Func()` will be merged.
 *
 *   using Ptr = decltype(&Func);
 *   NEX_NOMERGE Ptr ptr = &Func;  // No calls through `ptr` will be merged.
 *
 *   NEX_NOMERGE if (cond) {
 *       // No calls in this block will be merged.
 *   }
 * @endcode
 */
#if NEX_HAS_CPP_ATTRIBUTE(clang::nomerge)
    #define NEX_NOMERGE [[clang::nomerge]]
#else  // Compiler does not support [[nomerge]] attribute
    #define NEX_NOMERGE
#endif

/**
 * @def NEX_STACK_UNINITIALIZED
 * @brief Disable automatic variable initialization for performance
 * 
 * @details
 * When available, this attribute disables -ftrivial-auto-var-init=pattern for the annotated variable. 
 * This is useful in performance-critical code where automatic initialization causes measurable overhead.
 * 
 * @par Background:
 * The -ftrivial-auto-var-init=pattern flag writes a fixed pattern to uninitialized parts of all local 
 * variables. While this mitigates security risks, it can cause performance issues in specific scenarios.
 * 
 * @par Use Cases:
 * 1. **Performance-Critical Code**: When compiler-added initialization causes measurable performance 
 *    regression in benchmarks
 * 2. **Manual Initialization**: When code properly initializes memory later and the intermediate pattern 
 *    writes are wasteful
 * 3. **No C Runtime**: When code doesn't link with C runtime library and compiler might insert memset/memcpy 
 *    calls due to initialization
 * 
 * @warning 
 * This attribute is a security risk mitigation bypass. Use sparingly and document the specific performance 
 * issue. The goal is to minimize exceptions while maintaining security benefits across most code.
 * 
 * @par Library-Wide Alternative:
 * To disable for entire targets, use in .gn files:
 * @code
 * configs -= [ "//build/config/compiler:default_init_stack_vars" ]
 * @endcode
 * 
 * @see http://crbug.com/977230 for background on init_stack_vars
 * @see build/config/compiler/BUILD.gn for the init_stack_vars configuration
 * 
 * @note 
 * Please document platform, bot, benchmark, or test name when using this attribute to help future cleanup efforts.
 */
#if NEX_HAS_CPP_ATTRIBUTE(clang::uninitialized)
    #define NEX_STACK_UNINITIALIZED [[clang::uninitialized]]
#elif NEX_HAS_CPP_ATTRIBUTE(gnu::uninitialized)
    #define NEX_STACK_UNINITIALIZED [[gnu::uninitialized]]
#else  // Compiler does not support uninitialized attribute
    #define NEX_STACK_UNINITIALIZED
#endif  // NEX_STACK_UNINITIALIZED

/**
 * @def NEX_NO_STACK_PROTECTOR
 * @brief Disable stack protection canaries for a function
 * 
 * @details
 * Disables the -fstack-protector flag for the annotated function. Stack protection adds a canary value to 
 * each stack frame that is checked on function return to detect buffer overflows.
 * 
 * @par Stack Protection Mechanism:
 * When enabled, -fstack-protector adds a canary to each stack frame. On function return, the canary is compared 
 * against a reference value. Mismatches indicate a likely stack buffer overflow, triggering an immediate crash 
 * to prevent exploitation.
 * 
 * @par Use Cases:
 * - **Performance**: Hot functions where canary checks cause measurable overhead
 * - **Intentional Modification**: Functions that purposely change the reference canary
 * - **Process Forking**: Functions in the call stack leading to forked sub-processes where the child process 
 *   inherits the canary state, which can cause false positives if the child modifies the stack.
 * 
 * @see https://crbug.com/40181003 for details on fork behavior
 */
#if NEX_HAS_CPP_ATTRIBUTE(gnu::no_stack_protector)
    #define NEX_NO_STACK_PROTECTOR [[gnu::no_stack_protector]]
#elif NEX_HAS_CPP_ATTRIBUTE(gnu::optimize)
    #define NEX_NO_STACK_PROTECTOR [[gnu::optimize("-fno-stack-protector")]]
#else  // Compiler does not support [[no_stack_protector]] attribute
    #define NEX_NO_STACK_PROTECTOR
#endif  // NEX_NO_STACK_PROTECTOR

/**
 * @def NEX_REINITIALIZES_AFTER_MOVE
 * @brief Annotate a member function as safe to call on a moved-from object, which it will reinitialize.
 * 
 * @details
 * This macro expands to the appropriate compiler-specific attribute to indicate that a member function can be safely 
 * called on an object that has been moved from, and that the function will reinitialize the object to a valid state. 
 * This is particularly useful for functions that are intended to be called after an object has been moved from.
 * 
 * @see https://clang.llvm.org/extra/clang-tidy/checks/bugprone/use-after-move.html#reinitialization
 * 
 * Example usage:
 * @code
 *   struct S {
 *       NEX_REINITIALIZES_AFTER_MOVE void Reset();
 *   };
 *   void Func1(const S&);
 *   void Func2() {
 *       S s1;
 *       S s2 = std::move(s1);
 *       s1.Reset();
 *       // clang-tidy's `bugprone-use-after-move` check will not flag the
 *       // following call as a use-after-move, due to the intervening `Reset()`.
 *       Func1(s1);
 *   }
 * @endcode
 */
#if NEX_HAS_CPP_ATTRIBUTE(clang::reinitializes)
    #define NEX_REINITIALIZES_AFTER_MOVE [[clang::reinitializes]]
#else  // Compiler does not support [[reinitializes]] attribute, or we are not using Clang
    #define NEX_REINITIALIZES_AFTER_MOVE
#endif  // NEX_REINITIALIZES_AFTER_MOVE
