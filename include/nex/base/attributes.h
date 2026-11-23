/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    attributes.h
 * @brief   Defines portable compiler and language annotation macros.
 *
 * @details
 * This header provides a consistent set of annotation macros for use throughout NexCore.
 *
 * It abstracts compiler-specific and language-level attributes into a common interface,
 * including annotations related to deprecation, return-value usage, function inlining,
 * optimization hints, and other code-generation or diagnostic behaviors.
 *
 * The implementation adapts these annotations to the capabilities of the active compiler
 * and language mode.
 *
 * @note
 * Compiler feature detection and low-level attribute syntax abstractions are provided
 * by compiler_features.h.
 *
 * @see compiler_features.h
 */

#include "nex/base/compiler.h"
#include "nex/base/language.h"
#include "nex/base/compiler_features.h"
#include "nex/base/build.h"

/**
 * @def NEX_NOINLINE
 * @brief Annotates a function indicating it should not be inlined by the compiler.
 * 
 * @details
 * This macro can be used to prevent the compiler from inlining a function, which can be useful for debugging,
 * reducing code size, or ensuring that a function call boundary is preserved for performance analysis.
 * The macro expands to the appropriate compiler-specific attribute based on the detected compiler.
 * For example, it expands to `[[clang::noinline]]` for Clang, `[[gnu::noinline]]` for GCC, and `[[msvc::noinline]]`
 * for MSVC. If the compiler does not support a noinline attribute, it expands to nothing.
 *
 * @par Example usage:
 * @code
 *   NEX_NOINLINE void DoStuff() {
 *       // This body will not be inlined into callers.
 *   }
 * @endcode
 *
 * @see https://clang.llvm.org/docs/AttributeReference.html#noinline
 */
#if NEX_HAS_CPP_ATTRIBUTE(noinline)
    #define NEX_NOINLINE [[noinline]]
#elif NEX_HAS_CLANG_ATTRIBUTE(noinline)
    #define NEX_NOINLINE NEX_CLANG_ATTRIBUTE(noinline)
#elif NEX_HAS_GCC_ATTRIBUTE(noinline)
    #define NEX_NOINLINE NEX_GCC_ATTRIBUTE(noinline)
#elif NEX_HAS_MSVC_ATTRIBUTE(noinline)
    #define NEX_NOINLINE NEX_MSVC_ATTRIBUTE(noinline)
#else  // Compiler does not support [[noinline]] attribute
    #define NEX_NOINLINE
#endif  // ^^NEX_NOINLINE

/**
 * @def NEX_ALWAYS_INLINE
 * @brief Annotates a function indicating it should always be inlined.
 * 
 * @details
 * This macro can be used to suggest to the compiler that a function should always be inlined, which can
 * improve performance by eliminating function call overhead and enabling further compiler optimizations.
 * It expands to the appropriate compiler-specific attribute based on the detected compiler.
 * 
 * @note Since `NEX_ALWAYS_INLINE` is performance-oriented but can hamper debugging, ignore it in debug mode.
 * 
 * Example usage:
 * @code
 *   NEX_ALWAYS_INLINE void FastFunction() {
 *       // This body will be inlined into callers whenever possible.
 *   }
 * @endcode
 *
 * @see https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline
 */
#if NEX_BUILD_MODE_IS_RELEASE
    // In release builds, we can use the appropriate always-inline attribute for the compiler
    #if NEX_COMPILER_MSVC_COMPATIBLE
        // MSVC-specific always-inline directive
        #define NEX_ALWAYS_INLINE __forceinline
    #elif (NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE) && NEX_HAS_ATTRIBUTE(always_inline)
        // Clang and GCC support the always_inline attribute,
        // which can be used to suggest that a function should always be inlined
        #define NEX_ALWAYS_INLINE inline NEX_GCC_OR_CLANG_ATTRIBUTE(always_inline)
    #else  // Fallback for other compilers or when the always_inline attribute is not available
        #define NEX_ALWAYS_INLINE inline
    #endif
#else
    // In debug builds, we avoid forcing inlining
    // to preserve function boundaries for better debugging experience
    #define NEX_ALWAYS_INLINE inline
#endif  // ^^NEX_ALWAYS_INLINE

/**
 * @def NEX_WEAK_SYMBOL
 * @brief Marks a symbol as weak, allowing it to be overridden by other definitions.
 * 
 * @details
 * This macro expands to the appropriate compiler-specific attribute to mark a symbol as weak.
 * Weak symbols can be overridden by other definitions, which is useful for providing default
 * implementations that can be replaced by user-defined ones.
 * On compilers that do not support weak symbols, this macro expands to nothing.
 */
#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_WEAK_SYMBOL NEX_GCC_OR_CLANG_ATTRIBUTE(weak)
#else  // Compiler does not support weak symbols
    #define NEX_WEAK_SYMBOL
#endif  // ^^NEX_WEAK_SYMBOL

/**
 * @def NEX_MSVC_FLATTEN
 * @brief Annotates a function to suggest aggressive inlining and optimization across call boundaries in MSVC.
 * 
 * @details
 * This macro expands to `[[msvc::flatten]]` when compiling with MSVC if the compiler supports this attribute,
 * and expands to nothing on other compilers. The `[[msvc::flatten]]` attribute is a hint to the MSVC compiler
 * that it should consider inlining the annotated function and its callees into the caller, even across
 * multiple call boundaries.
 * This can enable more aggressive optimization and better performance in hot code paths, but it may also
 * increase compile times and code size, so it should be used judiciously on performance-critical functions.
 */
#if NEX_HAS_MSVC_ATTRIBUTE(flatten)
    #define NEX_MSVC_FLATTEN NEX_MSVC_ATTRIBUTE(flatten)
#else // Compiler does not support [[msvc::flatten]]
    #define NEX_MSVC_FLATTEN
#endif  // ^^NEX_MSVC_FLATTEN

/**
 * @def NEX_FALLTHROUGH
 * @brief Annotates intentional fallthrough in switch statements to suppress compiler warnings.
 * 
 * @details
 * This macro can be used to indicate that a fallthrough in a switch statement is intentional,
 * which can help suppress compiler warnings about implicit fallthrough.
 * The macro expands to the appropriate attribute syntax if the compiler supports the C++17
 * `[[fallthrough]]` attribute, and expands to nothing on compilers that do not support it
 * or when compiling with C++14 or earlier.
 * 
 * @par Example usage:
 * @code
 * switch (value) {
 *    case 1:
 *        // Handle case 1
 *        NEX_FALLTHROUGH   // Intentional fallthrough to case 2
 *    case 2:
 *        // Handle case 2
 *        break;
 * };
 * @endcode
 */
#if NEX_HAS_CPP_ATTRIBUTE(fallthrough) >= 201603L
    /**
     * @note
     * Include the semicolon inside the macro to prevent omission mistakes.
     * This allows callers to use `NEX_FALLTHROUGH` without a trailing semicolon,
     * keeping it safe and clean when unsupported and expanding to nothing.
     */
    #define NEX_FALLTHROUGH [[fallthrough]];
#else  // Compiler does not support [[fallthrough]] or we are in C++14 or earlier
    #define NEX_FALLTHROUGH
#endif  // ^^NEX_FALLTHROUGH

/**
 * @def NEX_NODISCARD/NEX_NODISCARD_MSG
 * @brief Marks functions/classes whose return values should not be discarded.
 * 
 * @details
 * These macros are used to indicate that the return value of a function should not be ignored.
 * This is useful for functions that return error codes, status objects, or other important information
 * that should be checked by the caller.
 * The NEX_NODISCARD_MSG variant allows for a custom message to be provided, which can give more context
 * about why the return value should not be discarded.
 */

#if !defined(NEX_HAS_NODISCARD)
    #if NEX_HAS_CPP_ATTRIBUTE(nodiscard) >= NEX_CXX_STD_17
        #define NEX_HAS_NODISCARD 1
    #elif NEX_COMPILER_MSVC_COMPATIBLE && NEX_COMPILER_MSVC_VER >= 1910
        // MSVC 2017 version 15.3 and later support [[nodiscard]]
        #define NEX_HAS_NODISCARD 1
    #else  // Compiler does not support [[nodiscard]] or we are in C++14 or earlier
        #define NEX_HAS_NODISCARD 0
    #endif  // NEX_HAS_NODISCARD
#endif  // !defined(NEX_HAS_NODISCARD)

#if NEX_HAS_NODISCARD
    #define NEX_NODISCARD [[nodiscard]]
#elif NEX_COMPILER_GCC_COMPATIBLE
    // GCC supports [[gnu::warn_unused_result]] to indicate that the return value should not be ignored
    #if NEX_HAS_GCC_ATTRIBUTE(warn_unused_result)
        #define NEX_NODISCARD NEX_GCC_ATTRIBUTE(warn_unused_result)
    #else  // GCC version does not support [[gnu::warn_unused_result]]
        /**
         * @note
         * Fallback to older GCC attribute syntax,
         * which also indicates that the return value should not be ignored.
         */
        #define NEX_NODISCARD NEX_GCC_OR_CLANG_ATTRIBUTE(warn_unused_result)
    #endif
#endif  // ^^NEX_HAS_NODISCARD

#if !defined(NEX_NODISCARD)
    #define NEX_NODISCARD   // No-op if NODISCARD is not supported
#endif  // ^^!defined(NEX_NODISCARD)

#if !defined(NEX_HAS_NODISCARD_MSG)
    #if NEX_HAS_CPP_ATTRIBUTE(nodiscard) >= 201907L
        #define NEX_HAS_NODISCARD_MSG 1
    #elif NEX_COMPILER_MSVC_COMPATIBLE && NEX_COMPILER_MSVC_VER >= 1925
        // MSVC 2019 version 16.5 and later support [[nodiscard("message")]]
        #define NEX_HAS_NODISCARD_MSG 1
    #else  // Compiler does not support [[nodiscard("message")]] or we are in C++17 or earlier
        #define NEX_HAS_NODISCARD_MSG 0
    #endif  // NEX_HAS_NODISCARD_MSG
#endif  // ^^!defined(NEX_HAS_NODISCARD_MSG)

#if NEX_HAS_NODISCARD_MSG
    #define NEX_NODISCARD_MSG(msg) [[nodiscard(msg)]]
#else  // Compiler does not support [[nodiscard("message")]]
    /**
     * @note
     * Fallback to NEX_NODISCARD without the additional message, since the compiler
     * does not support the message variant.
     * This will still indicate that the return value should not be discarded,
     * but the additional context provided by the message will be safely ignored.
     */
    #define NEX_NODISCARD_MSG(msg) NEX_NODISCARD
#endif  // ^^NEX_HAS_NODISCARD_MSG

/**
 * @def NEX_ALIGNAS(byte_alignment)
 * @brief Specifies memory alignment for types and variables.
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
 * class NEX_ALIGNAS(16) MyClass { ... };   // Align class to 16 bytes
 * NEX_ALIGNAS(16) int array[4];            // Align array to 16 bytes
 * @endcode
 * 
 * @par Compiler Compatibility:
 * The C++11 `alignas` keyword cannot always be mixed with compiler attributes:
 * - Does NOT work: `struct alignas(16) __attribute__((packed)) S { char c; };`
 * - Works in Clang only: `struct __attribute__((packed)) alignas(16) S { char c; };`
 * - Works in both: `struct alignas(16) S { char c; } __attribute__((packed));`
 * 
 * @note
 * Some attributes (like visibility for exported classes) must be specified
 * before the class definition, making `alignas` unusable in those cases.
 */
#if NEX_CXX_VER >= NEX_CXX_STD_11
    #define NEX_ALIGNAS(byte_alignment) alignas(byte_alignment)
#elif NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_ALIGNAS(byte_alignment) NEX_MSVC_DECLSPEC(align(byte_alignment))
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_ALIGNAS(byte_alignment) NEX_GCC_OR_CLANG_ATTRIBUTE(aligned(byte_alignment))
#endif  // ^^NEX_ALIGNAS

/**
 * @def NEX_NOALIAS
 * @brief Marks a function or pointer as not aliasing any other pointer.
 * 
 * @details
 * This macro expands to the appropriate compiler-specific attribute to indicate that a function or pointer
 * does not alias any other pointer. This can help the compiler optimize code by assuming that
 * the memory accessed through this pointer is not accessed through any other pointer.
 */
#if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_CLANG_COMPATIBLE
    #define NEX_NOALIAS NEX_GCC_OR_CLANG_ATTRIBUTE(__malloc__)
#else  // Compiler does not support noalias
    #define NEX_NOALIAS
#endif  // ^^NEX_NOALIAS

/**
 * @def NEX_CONSTEXPR23
 * @brief Specifies `constexpr` for functions when compiling with C++23 support.
 * 
 * @details
 * Use this to annotate functions that can be `constexpr` in C++23 but not in earlier standards.
 * This allows the same codebase to take advantage of C++23 features when available without
 * breaking compatibility with earlier C++ standards.
 * 
 * @note 
 * Some functions can only be marked as `constexpr` in C++23 but not in C++20 due to relaxed rules
 * around what is allowed in `constexpr` functions.
 * For example, C++23 allows for more complex control flow and certain standard library functions
 * to be used in `constexpr` contexts.
 * For functions that can be marked as `constexpr` in both C++17 and C++20 without causing any
 * compilation issues, simply use `constexpr` directly without this macro.
 *
 * @par Example usage:
 * @code
 * NEX_CONSTEXPR23 int Add(int a, int b) {
 *     return a + b;
 * }
 * @endcode
 */
#if NEX_HAS_CXX23
    #define NEX_CONSTEXPR23 constexpr
#else  // For C++20 and earlier
    #define NEX_CONSTEXPR23
#endif  // ^^NEX_CONSTEXPR23

/**
 * @def NEX_EXPLICIT23
 * @brief Specifies `explicit` for constructors when compiling with C++23 support.
 *
 * @details
 * Use this to annotate constructors that should be `explicit` in C++23 but may not be in earlier standards.
 * This allows the same codebase to take advantage of C++23 features when available without breaking compatibility
 * with earlier C++ standards.
 *
 * @par Example usage:
 * @code
 * struct Foo {
 *     NEX_EXPLICIT23 Foo(int x) {}
 * };
 * @endcode
 */
#if NEX_HAS_CXX23
    #define NEX_EXPLICIT23 explicit
#else  // For C++20 and earlier
    #define NEX_EXPLICIT23
#endif  // ^^NEX_EXPLICIT23

/**
 * @def NEX_CLANG_ENABLE_IF(...)
 * @brief Conditionally enables a function overload based on compile-time argument evaluation.
 * 
 * @details
 * The @c __attribute__((__enable_if__(condition, message))) extension (supported by Clang) allows you
 * to control overload resolution based on expressions involving the function's parameters,
 * evaluated at compile time.
 *
 * - If the condition evaluates to true, the function is a viable candidate for overload resolution.
 * - If the condition evaluates to false, the function is removed from the overload set (similar to
 *   C++ SFINAE or constraints).
 * - If a custom message is provided, it can be surfaced by the compiler as part of a diagnostic error
 *   if the call fails to match any valid overload.
 *
 * @note This is a Clang-specific extension and is not available on other compilers.
 */
#if NEX_HAS_ATTRIBUTE(__enable_if__)
    // Expands to `__attribute__((__enable_if__(condition, "message")))` on Clang,
    // which allows for conditional compilation based on a boolean expression.
    #define NEX_CLANG_ENABLE_IF(...) NEX_GCC_OR_CLANG_ATTRIBUTE(__enable_if__(__VA_ARGS__))
#else
    // Expands to nothing on compilers that do not support the `__enable_if__` attribute.
    #define NEX_CLANG_ENABLE_IF(...)
#endif  // ^^NEX_CLANG_ENABLE_IF(...)

/**
 * @def NEX_USING_IF_EXISTS
 * @brief Marks a declaration to be used only if it exists.
 *
 * @details
 * The @c __attribute__((__using_if_exists__)) extension (supported by Clang and GCC) allows you
 * to indicate that a declaration should only be used if it exists, helping to avoid compilation
 * errors when dealing with optional or conditionally available entities.
 *
 * @note This is a compiler-specific extension and is not available on all compilers.
 */
#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_IS_GCC_COMPATIBLE
    #define NEX_USING_IF_EXISTS NEX_GCC_OR_CLANG_ATTRIBUTE(__using_if_exists__)
#else  // Compiler does not support __attribute__((__using_if_exists__))
    #define NEX_USING_IF_EXISTS
#endif  // ^^NEX_USING_IF_EXISTS

/**
 * @def NEX_MSVC_INTRINSIC
 * @brief Tells the MSVC compiler to treat a single-parameter cast function as an intrinsic.
 *
 * @details
 * The @c [[msvc::intrinsic]] attribute (specific to the Microsoft Visual C++ compiler)
 * instructs the compiler to replace all calls to the annotated function directly with
 * its underlying type-cast expression, effectively eliminating function call overhead.
 *
 * Strict requirements apply to functions marked with this attribute:
 * - The function must take exactly **one parameter**.
 * - The function must **not be recursive**.
 * - The function body must consist solely of a single `return` statement containing a `static_cast`
 *   from the parameter type to the return type.
 *
 * @note
 * This is a Microsoft-specific extension supported in Visual Studio 2022 (v17.5) and later, and requires
 * the @c /permissive- compiler flag (which is enabled by default in C++20 and newer standards).
 */
#if NEX_HAS_MSVC_ATTRIBUTE(intrinsic)
    #define NEX_MSVC_INTRINSIC NEX_MSVC_ATTRIBUTE(intrinsic)
#else  // Compiler does not support [[msvc::intrinsic]]
    #define NEX_MSVC_INTRINSIC
#endif  // ^^NEX_MSVC_INTRINSIC

/**
 * @def NEX_MSVC_KNOWN_SEMANTICS
 * @brief Instructs the MSVC compiler that an entity strictly adheres to standard-mandated semantics.
 *
 * @details
 * The @c [[msvc::known_semantics]] attribute is an internal, Microsoft-specific extension used
 * primarily within the MSVC Standard Library (STL) implementation.
 * It informs the compiler that a specific type trait, template specialization, or helper alias
 * follows well-defined standard semantics. 
 *
 * This allows the compiler frontend to recognize and optimize or evaluate the construct natively
 * (similar to an intrinsic type trait), significantly improving template instantiation performance
 * and overall compilation throughput.
 *
 * @note
 * This is an internal compiler-vendor attribute designed specifically for the MSVC toolchain
 * and standard library headers. It is not part of the standard C++ language specification
 * and is generally not intended for direct use in user-written application code.
 */
#if NEX_HAS_MSVC_ATTRIBUTE(known_semantics)
    #define NEX_MSVC_KNOWN_SEMANTICS NEX_MSVC_ATTRIBUTE(known_semantics)
#else  // Compiler does not support [[msvc::known_semantics]]
    #define NEX_MSVC_KNOWN_SEMANTICS
#endif  // ^^NEX_MSVC_KNOWN_SEMANTICS

/**
 * @def NEX_DECLSPEC_EMPTY_BASES
 * @brief Marks a class as having empty base optimization (EBO) when supported by the compiler.
 *
 * @details
 * The @c __declspec(empty_bases) attribute is a Microsoft-specific extension that allows the compiler
 * to optimize the memory layout of classes with empty base classes, potentially reducing the overall
 * size of the derived class.
 *
 * @note This is a compiler-specific extension and is not available on all compilers.
 */
#if NEX_HAS_DECLSPEC_ATTRIBUTE(empty_bases)
    #define NEX_DECLSPEC_EMPTY_BASES NEX_DECLSPEC_ATTRIBUTE(empty_bases)
#else  // Compiler does not support __declspec(empty_bases)
    #define NEX_DECLSPEC_EMPTY_BASES
#endif  // ^^NEX_DECLSPEC_EMPTY_BASES

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

/**
 * @def NEX_NO_UNIQUE_ADDRESS
 * @brief Instructs the compiler that a data member does not need to have a distinct address from other members.
 *
 * @details
 * Introduced in C++20, the @c [[no_unique_address]] attribute is primarily used for optimizing memory layout
 * by eliminating space wasted on empty classes (such as stateless custom allocators, functors, or base classes
 * using the Empty Base Optimization - EBO).
 *
 * - If applied to a non-static data member of an empty class type, the compiler is permitted to place another
 *   member or subobject at the same memory address, reducing the overall size of the containing class.
 * - Unlike standard empty base optimization, this works directly on data members rather than inheritance hierarchies.
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
 * @note
 * Supported in C++20 and later standards.
 * Note that MSVC prior to certain updates (or depending on data member layout rules) handles its ABI representation
 * differently compared to GCC and Clang.
 */
#if NEX_HAS_MSVC_ATTRIBUTE(no_unique_address)
    #define NEX_NO_UNIQUE_ADDRESS NEX_MSVC_ATTRIBUTE(no_unique_address)
#elif NEX_HAS_CPP_ATTRIBUTE(no_unique_address)
    #define NEX_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else  // Compiler does not support [[no_unique_address]]
    #define NEX_NO_UNIQUE_ADDRESS
#endif  // ^^NEX_NO_UNIQUE_ADDRESS

/**
 * @brief Mark a function as not returning (i.e., it will not return to the caller)
 * 
 * @details
 * This macro can be used to indicate that a function does not return to the caller, which can help 
 * the compiler optimize code and generate better warnings. For example, functions that throw exceptions, 
 * call `std::exit()`, or enter an infinite loop can be marked with this attribute. The macro expands 
 * to the appropriate compiler-specific attribute based on the detected compiler. If the compiler does not 
 * support a noreturn attribute, the macro expands to nothing, allowing the code to compile without errors.
 */
/**
 * @def NEX_NORETURN
 * @brief Specifies that a function does not return to its caller.
 *
 * @details
 * The @c [[noreturn]] attribute (introduced in C++11) indicates that a function terminates
 * the control flow without returning control back to the calling function.
 * This typically applies to functions that throw exceptions, terminate the process, or execute
 * an infinite loop.
 *
 * - It enables the compiler to optimize control flow and suppress false-positive warnings regarding
 *   unreachable code or missing return statements.
 * - If a function marked @c [[noreturn]] actually returns control (e.g., execution reaches the end
 *   of the function body), the behavior is **undefined**.
 *
 * @note
 * Standard C++11 attribute syntax. For older codebases, equivalent pre-C++11 extensions
 * include @c __attribute__((noreturn)) (GCC/Clang) or @c __declspec(noreturn) (MSVC).
 */
#if NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_NORETURN NEX_MSVC_DECLSPEC(noreturn)
#elif NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_NORETURN NEX_GCC_OR_CLANG_ATTRIBUTE(noreturn)
#else  // Compiler does not support noreturn attribute
    #define NEX_NORETURN
#endif  // ^^NEX_NORETURN

/**
 * @def NEX_NODEBUG
 * @brief Instructs the compiler to omit debug information for a function, variable, or type.
 *
 * @details
 * The @c [[gnu::nodebug]] attribute (equivalent to the GNU extension @c __attribute__((nodebug))) tells GCC
 * and Clang not to generate debug symbols (such as DWARF debugging entries) for the annotated entity.
 *
 * - **Reduced Symbol Size:** This is useful for shrinking the size of debug symbol files (e.g., binaries
 *   or external debug info files) by stripping out symbols for uninteresting code.
 * - **Compilation Performance:** It can reduce compiler overhead for massive, repetitive inline helpers
 *   or template-heavy code paths where debug traces are unnecessary.
 * - **Debugger Behavior:** If a debugger steps into a function marked with this attribute, it may skip
 *   over it or treat it as part of the caller's frame.
 *
 * @note
 * This is a GCC and Clang compiler extension, written using standard C++11 attribute syntax with
 * the @c gnu:: namespace prefix.
 */
#if NEX_HAS_GCC_ATTRIBUTE(nodebug)
    #define NEX_NODEBUG NEX_GCC_ATTRIBUTE(nodebug)
#else  // Compiler does not support [[gnu::nodebug]]
    #define NEX_NODEBUG
#endif  // NEX_NODEBUG

/**
 * @def NEX_DEPRECATED / NEX_DEPRECATED_MSG(msg)
 * @brief Marks an entity (such as a function, class, or variable) as deprecated.
 *
 * @details
 * Introduced in C++14, the @c [[deprecated]] attribute (and its optional string literal message)
 * instructs the compiler to issue a warning whenever the annotated entity is referenced in user code.
 *
 * - **Migration Guidance:** The optional string message provides developers with context regarding
 *   why the feature is deprecated and what replacement to use instead.
 * - **Deprecation Lifecycle:** This serves as a formal notice before eventual removal in a future release,
 *   helping maintainers gracefully phase out legacy APIs.
 *
 * @par Example usage:
 * @code
 *   NEX_DEPRECATED("Use NewFunction() instead") void OldFunction() { ... }
 * @endcode
 *
 * @note
 * Standard C++14 attribute syntax.
 * For older C++11 codebases, pre-standard equivalents like @c __attribute__((deprecated("message"))) (GCC/Clang)
 * or @c __declspec(deprecated("message")) (MSVC) are commonly used via preprocessor wrappers.
 */
#if NEX_HAS_CPP_ATTRIBUTE(deprecated) >= NEX_CXX_STD_14
    #define NEX_DEPRECATED [[deprecated]]
    #define NEX_DEPRECATED_MSG(msg) [[deprecated(msg)]]
#elif NEX_COMPILER_MSVC_COMPATIBLE
    #define NEX_DEPRECATED NEX_MSVC_DECLSPEC(deprecated)
    #define NEX_DEPRECATED_MSG(msg) NEX_MSVC_DECLSPEC(deprecated(msg))
#elif NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE || NEX_HAS_ATTRIBUTE(deprecated)
    #define NEX_DEPRECATED NEX_GCC_OR_CLANG_ATTRIBUTE(deprecated)
    #define NEX_DEPRECATED_MSG(msg) NEX_GCC_OR_CLANG_ATTRIBUTE(deprecated(msg))
#else  // Compiler does not support deprecation attribute
    #define NEX_DEPRECATED
    #define NEX_DEPRECATED_MSG(msg)
#endif  // ^^NEX_DEPRECATED / NEX_DEPRECATED_MSG

/**
 * @def NEX_MAYBE_UNUSED
 * @brief Suppresses compiler warnings for entities that may legitimately go unused.
 *
 * @details
 * Introduced in C++17, the @c [[maybe_unused]] attribute instructs the compiler that a variable,
 * function, class, struct, typedef, or enumerator might not be used in certain configurations,
 * code paths, or build modes.
 *
 * - **Warning Suppression:** It prevents the compiler from emitting unused-entity warnings
 *.  (such as @c -Wunused-variable, @c -Wunused-function, or @c -Wunused-private-field)
 *   without requiring awkward workarounds like casting to @c void.
 * - **Common Use Cases:** Ideal for variables or functions reserved for assertions,
 *   debugging logs, conditionally compiled code blocks (e.g., inside @c \#if macros),
 *   or parameters overridden in derived classes but unused in base implementations.
 *
 * @par Example usage:
 * @code
 *   NEX_MAYBE_UNUSED void HelperFunction() { ... }
 *   void MainFunction(int param) {
 *       NEX_MAYBE_UNUSED int unusedVar = 42;
 *       // ...
 *   }
 * @endcode
 *
 * @note
 * Standard C++17 attribute syntax.
 * For older C++11/14 codebases, pre-standard equivalents include @c __attribute__((unused)) (GCC/Clang)
 * or compiler-specific workarounds.
 */
#if NEX_HAS_CPP_ATTRIBUTE(maybe_unused) >= 201603L
    #define NEX_MAYBE_UNUSED [[maybe_unused]]
#else  // Compiler does not support [[maybe_unused]] or we are in C++14 or earlier
    #define NEX_MAYBE_UNUSED
#endif  // ^^NEX_MAYBE_UNUSED

// Marks a parameter as maybe unused to avoid compiler warnings.
#define NEX_UNUSED_PARAM(param) (void)(param)

// Marks a variable as maybe unused to avoid compiler warnings.
#define NEX_UNUSED_VAR(var) (void)(var)

/** 
 * @def NEX_UNUSED
 * @brief Marks multiple variables or parameters as maybe unused to avoid compiler warnings.
 * 
 * @details
 * This macro provides a unified, cross-platform way to suppress "unused variable" warnings.
 * It adapts its implementation based on the available C++ standard to ensure maximum efficiency
 * and compatibility.
 *
 * - In C++17 and later: It utilizes a variadic template function (@ref NEX_UNUSED_FUNC).
 *   By passing the variables as forwarding references to an inline empty function, the compiler marks
 *   them as "accessed." Modern compilers will optimize this call away entirely, resulting in zero
 *   runtime overhead.
 * - Pre-C++17: It employs a compile-time "sizeof" trick. It creates a zero-cost temporary array
 *   (compound literal) where each element is initialized by the variable being suppressed.
 *   Since sizeof is evaluated during compilation, no machine code is generated for the array at runtime.
 * 
 * @par Example usage:
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
#else  // For C++14 or earlier
    #define NEX_UNUSED(...) \
        (void)(sizeof((int[]){(NEX_UNUSED_VAR(__VA_ARGS__), 0)...}));
#endif  // ^^NEX_UNUSED

/**
 * @def NEX_UNLIKELY(x)
 * @brief Hints to the compiler that a specific branch or code path is statistically unlikely to be executed.
 *
 * @details
 * Introduced in C++20 alongside @c [[likely]], the @c [[unlikely]] attribute allows developers to provide
 * optimization hints to the compiler regarding control flow frequencies.
 *
 * - **Code Layout & Optimization:** It helps the compiler optimize branch prediction and reorder basic
 *   blocks (moving "cold" paths out-of-line) to improve instruction cache locality.
 * - **Placement:** Can be applied to individual @c if branch statements, @c switch case labels, or
 *   general execution statements.
 * - **Profiling Note:** These attributes should ideally be guided by profiling data (PGO), as incorrect
 *   manual hints can sometimes degrade performance rather than improve it.
 *
 * @param x The expression to evaluate
 * @return The boolean result of the expression
 *
 * @note
 * Standard C++20 attribute syntax.
 * For pre-C++20 codebases, compiler-specific built-ins like @c __builtin_expect() (GCC/Clang)
 * are commonly used instead.
 */
#if !defined(NEX_UNLIKELY)
    #if NEX_HAS_CXX20
        #define NEX_UNLIKELY(x) (x) [[unlikely]]
    #elif NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG_BASED
        #define NEX_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #else  // Compiler does not support __builtin_expect
        #define NEX_UNLIKELY(x) (x)
    #endif  // ^^NEX_HAS_CXX20
#endif  // ^^NEX_UNLIKELY

/**
 * @def NEX_LIKELY(x)
 * @brief Hints to the compiler that a specific branch or code path is statistically likely to be executed.
 *
 * @details
 * Introduced in C++20 alongside @c [[unlikely]], the @c [[likely]] attribute allows developers to provide
 * optimization hints to the compiler regarding control flow frequencies.
 *
 * - **Code Layout & Optimization:** It helps the compiler optimize branch prediction and layout
 *   the code such that the most frequently taken path remains straight-line execution, improving
 *   instruction cache locality.
 * - **Placement:** Can be applied to individual @c if branch statements, @c switch case labels,
 *   or general execution statements.
 * - **Profiling Note:** These attributes should ideally be guided by profiling data (PGO), as
 *   incorrect manual hints can sometimes degrade performance rather than improve it.
 *
 * @param x The expression to evaluate
 * @return The boolean result of the expression
 *
 * @note
 * Standard C++20 attribute syntax.
 * For pre-C++20 codebases, compiler-specific built-ins like @c __builtin_expect() (GCC/Clang)
 * are commonly used instead.
 */
#if !defined(NEX_LIKELY)
    #if NEX_HAS_CXX20
        #define NEX_LIKELY(x) (x) [[likely]]
    #elif NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG_BASED
        #define NEX_LIKELY(x) __builtin_expect(!!(x), 1)
    #else  // Compiler does not support __builtin_expect
        #define NEX_LIKELY(x) (x)
    #endif  // ^^NEX_HAS_CXX20
#endif  // ^^NEX_LIKELY

/**
 * @def NEX_EXPECTS(expr)
 * @brief Specifies a contract precondition that must hold true when entering a function.
 *
 * @details
 * The @c [[expects(expr)]] attribute (part of the C++ Contracts feature set) is used to enforce
 * design-by-contract programming by defining clear preconditions that must be satisfied before
 * a function's body begins execution.
 *
 * - **Precondition Verification:** The expression @p expr is evaluated upon entering the function.
 * - **Contract Violation:** If the condition evaluates to @c false, a contract violation handler
 *   is invoked (typically leading to program termination or a custom log), depending on
 *   the build configuration and enforcement levels.
 * - **Documentation & Analysis:** It serves as formal, machine-readable API documentation
 *   and provides valuable optimization/static-analysis hints to the compiler.
 *
 * @par Example usage:
 * @code
 *   void ProcessData(int* data) {
 *       NEX_EXPECTS(data != nullptr);  // Precondition: data must not be null
 *       // Function logic...
 *   }
 * @endcode
 *
 * @note
 * This is part of the C++ Contracts specification.
 * Syntax and level of compiler support may vary depending on your toolchain and whether
 * experimental contract flags are enabled.
 */
#if NEX_HAS_CPP_ATTRIBUTE(expects) >= 201907L
    #define NEX_EXPECTS(expr) [[expects(expr)]]
#else  // Compiler does not support [[expects]] or we are in C++17 or earlier
    #define NEX_EXPECTS(expr)
#endif  // ^^NEX_EXPECTS

/**
 * @def NEX_NOMERGE
 * @brief Prevents the compiler from merging duplicate calls or statements during optimization.
 *
 * @details
 * The @c [[nomerge]] attribute (a Clang-specific extension, typically written as @c [[clang::nomerge]]
 * or as a statement attribute) instructs the optimizer not to merge identical call expressions or
 * combine tail calls.
 *
 * - **Optimization Prevention:** By default, compilers may optimize binaries by combining
 *   identical function calls or blocks into a single shared call location (tail merging).
 *   @c [[nomerge]] forces the compiler to keep these call sites distinct.
 * - **Debugging & Tracing:** This is especially useful for profiling, sanitizers, or
 *   debugging scenarios where distinct call-site locations and accurate, un-merged stack
 *   traces must be preserved independently.
 * - **Placement:** It can be applied directly to function declarations or as a statement
 *   attribute preceding a statement containing call expressions.
 *
 * @par Usage example:
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
 *
 * @note
 * This is a Clang/LLVM-specific compiler extension and is not part of the standard C++ language specification.
 * 
 * @see https://clang.llvm.org/docs/AttributeReference.html#nomerge
 */
#if NEX_HAS_CLANG_ATTRIBUTE(nomerge)
    #define NEX_NOMERGE NEX_CLANG_ATTRIBUTE(nomerge)
#else  // Compiler does not support [[nomerge]] attribute
    #define NEX_NOMERGE
#endif  // ^^NEX_NOMERGE

/**
 * @def NEX_STACK_UNINITIALIZED
 * @brief Disables automatic variable initialization for performance-critical local variables.
 *
 * @details
 * When available, this attribute disables the compiler's automatic stack variable initialization
 * (such as @c -ftrivial-auto-var-init=pattern) for the annotated variable. 
 *
 * - **Background:** While compiler flags like @c -ftrivial-auto-var-init mitigate security risks
 *   by writing a fixed pattern to uninitialized stack memory, they can introduce measurable
 *   performance overhead in hot code paths.
 * - **Use Cases:**
 *   - **Performance-Critical Code:** Bypassing compiler-added initialization that causes measurable
 *     regressions in benchmarks.
 *   - **Manual Initialization:** Avoiding redundant intermediate writes when code explicitly initializes
 *     memory later.
 *   - **No C Runtime:** Preventing the compiler from injecting unexpected @c memset or @c memcpy calls.
 *
 * @warning
 * This attribute bypasses important security risk mitigations.
 * Use it sparingly, document the specific performance issue, and include the platform, bot, benchmark,
 * or test name to assist future cleanup efforts.
 *
 * @par Library-Wide Alternative:
 * To disable stack variable initialization for entire build targets, update your @c .gn files:
 * @code
 *   configs -= [ "//build/config/compiler:default_init_stack_vars" ]
 * @endcode
 *
 * @see http://crbug.com/977230 for background on @c init_stack_vars.
 * @see build/config/compiler/BUILD.gn for the corresponding configuration.
 */
#if NEX_HAS_CLANG_ATTRIBUTE(uninitialized)
    #define NEX_STACK_UNINITIALIZED NEX_CLANG_ATTRIBUTE(uninitialized)
#elif NEX_HAS_GCC_ATTRIBUTE(uninitialized)
    #define NEX_STACK_UNINITIALIZED NEX_GCC_ATTRIBUTE(uninitialized)
#else  // Compiler does not support uninitialized attribute
    #define NEX_STACK_UNINITIALIZED
#endif  // ^^NEX_STACK_UNINITIALIZED

/**
 * @def NEX_NO_STACK_PROTECTOR
 * @brief Disables stack protection canaries for an annotated function.
 *
 * @details
 * When available, this attribute disables the compiler's stack protection flag (such as @c -fstack-protector)
 * for a specific function.
 *
 * - **Stack Protection Mechanism:** Normally, stack protection adds a security canary value to the
 *   stack frame, which is checked upon function return. A mismatch indicates a buffer overflow and
 *   triggers an immediate crash to prevent exploitation.
 * - **Use Cases:**
 *   - **Performance:** In hot, performance-critical functions where stack canary prologue/epilogue checks
 *     cause measurable overhead.
 *   - **Intentional Modification:** In low-level functions that purposely manipulate the stack or
 *     reference canary values.
 *   - **Process Forking:** In functions leading to forked child processes, where inheriting a master
 *     canary state can cause false-positive crashes if modified.
 *
 * @warning
 * Bypassing stack protection reduces security mitigations against buffer overflows.
 * Use this macro strictly and only when performance metrics or specific process architectures require it.
 *
 * @see https://crbug.com/40181003 for details on process fork behavior and stack canary implications.
 */
#if NEX_HAS_GCC_ATTRIBUTE(no_stack_protector)
    #define NEX_NO_STACK_PROTECTOR NEX_GCC_ATTRIBUTE(no_stack_protector)
#elif NEX_HAS_GCC_ATTRIBUTE(optimize)
    #define NEX_NO_STACK_PROTECTOR NEX_GCC_ATTRIBUTE(optimize("-fno-stack-protector"))
#else  // Compiler does not support [[no_stack_protector]] attribute
    #define NEX_NO_STACK_PROTECTOR
#endif  // ^^NEX_NO_STACK_PROTECTOR

/**
 * @def NEX_REINITIALIZES_AFTER_MOVE
 * @brief Annotate a member function as safe to call on a moved-from object, which it will reinitialize.
 * 
 * @details
 * This macro expands to the appropriate compiler-specific attribute to indicate that a member function can be safely 
 * called on an object that has been moved from, and that the function will reinitialize the object to a valid state. 
 * This is particularly useful for functions that are intended to be called after an object has been moved from.
 * 
 */
/**
 * @def NEX_REINITIALIZES_AFTER_MOVE
 * @brief Indicates that a member function re-initializes an object to a valid state.
 *
 * @details
 * The @c [[clang::reinitializes]] attribute is a Clang-specific extension. It is applied
 * to non-static, non-const member functions (such as @c reset(), @c clear(), or @c init())
 * that reset an object's internal state.
 *
 * - **Static Analysis & Use-After-Move:** Static code analyzers (like Clang-Tidy's
 *   @c bugprone-use-after-move check) track whether objects are used after being moved from.
 *   Annotating a reset method with this attribute informs the analyzer that the object is
 *   safely re-initialized and can be used again, suppressing false-positive warnings.
 * - **Contract & Intent:** It serves as clear documentation that the function is explicitly
 *   designed to bring a previously consumed, cleared, or invalid object back to a fully usable state.
 *
 * @par Example usage:
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
 *
 * @see https://clang.llvm.org/extra/clang-tidy/checks/bugprone/use-after-move.html#reinitialization
 */
#if NEX_HAS_CLANG_ATTRIBUTE(reinitializes)
    #define NEX_REINITIALIZES_AFTER_MOVE NEX_CLANG_ATTRIBUTE(reinitializes)
#else  // Compiler does not support [[reinitializes]] attribute, or we are not using Clang
    #define NEX_REINITIALIZES_AFTER_MOVE
#endif  // ^^NEX_REINITIALIZES_AFTER_MOVE

/**
 * @def NEX_LIFETIMEBOUND
 * @brief Specifies that a returned reference or pointer is bound to the lifetime of a parameter.
 *
 * @details
 * This attribute (supported as @c [[clang::lifetimebound]] in Clang and @c [[msvc::lifetimebound]]
 * in MSVC 17.7 and later) informs compilers and static code analyzers that a returned value
 * or an implicit @c this object depends on the lifetime of a specific parameter.
 *
 * - **Dangling Reference Prevention:** Both toolchains use this attribute to detect potential
 *   dangling pointer or reference bugs at compile time, particularly when temporary objects
 *   are passed as constructor arguments or function parameters.
 * - **Common Use Cases:** Widely utilized in container accessors, proxy objects, view types
 *   (such as @c std::string_view), and builder patterns where a reference must remain valid
 *   only as long as the underlying argument's storage lasts.
 *
 * @note Supported across modern Clang and MSVC compilers.
 *
 * @see https://clang.llvm.org/docs/AttributeReference.html#lifetimebound
 * @see https://learn.microsoft.com/en-us/cpp/code-quality/c26815?view=msvc-170
 */
#if NEX_HAS_CLANG_ATTRIBUTE(lifetimebound)
    #define NEX_LIFETIMEBOUND NEX_CLANG_ATTRIBUTE(lifetimebound)
#elif NEX_HAS_MSVC_ATTRIBUTE(lifetimebound)
    #define NEX_LIFETIMEBOUND NEX_MSVC_ATTRIBUTE(lifetimebound)
#else  // Compiler does not support [[lifetimebound]] attribute
    #define NEX_LIFETIMEBOUND
#endif  // ^^NEX_LIFETIMEBOUND

/**
 * @def NEX_NO_SANITIZE(...)
 * @brief Disables specific compiler sanitizers (such as ASan, TSan, or UBSan) for a function.
 *
 * @details
 * The @c no_sanitize attribute instructs code generation to omit specific sanitizer instrumentations
 * for the annotated function. 
 *
 * - **Supported Sanitizers:** Accepts strings matching sanitizer flags (e.g., @c "address", @c "thread",
 *   @c "undefined", @c "memory", @c "hwaddress").
 * - **Use Cases:** Essential for low-level system code, custom memory allocators, manual stack manipulation,
 *   or intentional behavior that would otherwise trigger false positives or unwanted checks under sanitization tools.
 * - **Compiler Support:** Available via Clang (@c [[clang::no_sanitize(...)]] or @c __attribute__((no_sanitize(...))))
 *   and GCC (@c __attribute__((no_sanitize(...)))).
 *   Specific older sanitizers also use legacy attributes like @c __attribute__((no_sanitize_address)).
 *
 * @warning
 * Disabling sanitizers removes safety checks for the annotated scope. Use only when dealing with deliberate
 * low-level exceptions where instrumentation causes false positives or critical performance regressions.
 *
 * @see https://clang.llvm.org/docs/AttributeReference.html#no_sanitize
 * @see https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html#no-sanitize
 */
#if !defined(NEX_NO_SANITIZE)
    #if NEX_HAS_CLANG_ATTRIBUTE(no_sanitize)
        #define NEX_NO_SANITIZE(...) NEX_CLANG_ATTRIBUTE(no_sanitize(__VA_ARGS__))
    #elif NEX_HAS_GCC_ATTRIBUTE(no_sanitize)
        #define NEX_NO_SANITIZE(...) NEX_GCC_ATTRIBUTE(no_sanitize(__VA_ARGS__))
    #elif NEX_HAS_ATTRIBUTE(no_sanitize)
        #define NEX_NO_SANITIZE(...) NEX_GCC_OR_CLANG_ATTRIBUTE(no_sanitize(__VA_ARGS__))
    #else  // Compiler does not support [[no_sanitize]] attribute
        #define NEX_NO_SANITIZE(...)
    #endif  // ^^NEX_NO_SANITIZE
#endif  // ^^!defined(NEX_NO_SANITIZE)

/**
 * @def NEX_NO_CFI
 * @brief Disables Control Flow Integrity (CFI) checks for a function.
 *
 * @details
 * The @c no_sanitize("cfi") attribute instructs the compiler to omit CFI instrumentation
 * for the annotated function. This is useful in low-level system code or when dealing
 * with deliberate exceptions where CFI checks may cause false positives or performance issues.
 *
 * @note Not supported on GCC.
 *
 * @see https://clang.llvm.org/docs/AttributeReference.html#no_sanitize
 */
#if NEX_HAS_ATTRIBUTE(no_sanitize) && !NEX_COMPILER_IS_GCC
    #define NEX_NO_CFI NEX_NO_SANITIZE("cfi")
#else  // Compiler is GCC or does not support CFI
    #define NEX_NO_CFI
#endif  // ^^NEX_NO_CFI

/**
 * @def NEX_NO_PTRAUTH_VTABLE_POINTER
 * @brief Disables pointer authentication for vtable pointers.
 *
 * @details
 * The @c __ptrauth_vtable_pointer__ attribute instructs the compiler to omit pointer authentication
 * for vtable pointers in the annotated function or class. This is useful in low-level system code
 * or when dealing with deliberate exceptions where pointer authentication may cause false positives
 * or performance issues.
 *
 * @note Only supported on Clang.
 */
#if NEX_HAS_CLANG_ATTRIBUTE(__ptrauth_vtable_pointer__)
    #define NEX_NO_PTRAUTH_VTABLE_POINTER(...) NEX_CLANG_ATTRIBUTE(__ptrauth_vtable_pointer__(__VA_ARGS__))
#else  // Compiler is not Clang or does not support __ptrauth_vtable_pointer__
    #define NEX_NO_PTRAUTH_VTABLE_POINTER(...)
#endif  // ^^NEX_NO_PTRAUTH_VTABLE_POINTER
