/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/platform.h"
#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/build.h"
#include "nex/base/namespace.h"
#include "nex/base/location.h"
#include "nex/base/types.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include <intrin.h> // For __fastfail on MSVC
#endif

// ======================================================================================
// Internal source location metadata for Assert/Crash handling
// ======================================================================================

// Get the relative file path for assertion and fatal handlers
#define NEX_ASSERT_FILE_PATH \
    (stripFilePath(NEX_SOURCE_FILE_PATH))

// Get function name for assertion and fatal handlers
#define NEX_ASSERT_FUNCTION_NAME \
    NEX_SOURCE_FUNCTION_NAME

// ======================================================================================
// Assert/Crash handling APIs
// ======================================================================================

NEX_NAMESPACE_BEGIN

/**
 * @brief Fatal handler function pointer type
 * 
 * @details
 * Called when a fatal error is triggered. 
 * The handler receives:
 *  - reason: Optional reason for the fatal error (cstring)
 *  - file: Source file where the fatal error occurred (cstring)
 *  - line: Line number in the source file (int32)
 *  - function: Function name where the fatal error occurred (cstring)
 * 
 * @example
 * ```
 * void myFatalHandler(cstring reason, cstring file, int32 line, cstring function) {
 *     // Custom fatal handling logic
 * }
 * ::NEX_PREPEND_NAMESPACE(setFatalHandler)(myFatalHandler);
 * ```
 */
using FatalHandler = void(*)(
    cstring reason,
    cstring file,
    int32 line,
    cstring function
);

/**
 * @brief Set the global fatal handler
 * 
 * @param handler Function pointer to handle fatal errors. If nullptr, uses default handler.
 * 
 * @note The default handler performs a low-level crash.
 */
void setFatalHandler(FatalHandler handler);

/**
 * @brief Get the current fatal handler
 * 
 * @return Current fatal handler function pointer. Returns default handler if no custom handler is set.
 */
FatalHandler getFatalHandler();

/**
 * @brief Immediately crash the program in a platform-specific manner
 * 
 * @details
 * This function triggers an immediate crash using a low-level trap instruction.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * @param reason Optional reason for the crash (currently unused)
 * 
 * @example
 * ```
 * if (unexpected_condition) {
 *     immediateCrash("Unexpected condition encountered");
 * }
 * ```
 */
NEX_NORETURN inline void immediateCrash() {
#if NEX_COMPILER_IS_MSVC
    // MSVC: __fastfail is a low-level intrinsic that triggers a fast fail, 
    // which is a special kind of crash that is designed to be fatal, unique, and non-allocating. 
    // It does not generate any code on its own, so we also need to trigger a trap or abort 
    // to ensure the program does not continue executing.
    __fastfail(0); // or __fastfail(FAST_FAIL_ILLEGAL_INSTRUCTION);

#elif NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG || defined(__INTEL_COMPILER)
    // GCC / Clang / ICC / AppleClang: trap immediately (illegal instruction)
    __builtin_trap();

#else
    // Fallback: cause a crash by dereferencing a null pointer with an invalid value.
    // This is not guaranteed to be unique or non-allocating, but it's better than nothing 
    // on unsupported platforms.
    *(volatile int32*)0 = 0xDEADC0DE;
#endif

    // The function is marked [[noreturn]], so we should never reach this point.
    // If we do, it's an indication that the platform-specific crash mechanism failed 
    // to terminate the program, which is a critical failure. In that case, 
    // we can call std::abort() as a last resort to ensure the program 
    // does not continue executing in an undefined state.
#if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
    __builtin_unreachable();
#endif
}

/**
 * @brief Indicate that a code path is unreachable
 * 
 * @details
 * This function marks a code path as unreachable, allowing the compiler
 * to optimize accordingly. It uses compiler-specific extensions to
 * indicate that the code path should never be executed.
 * 
 * @example
 * ```
 * switch (value) {
 *     case 1:
 *         // Handle case 1
 *         break;
 *     case 2:
 *         // Handle case 2
 *         break;
 *     default:
 *         unreachable(); // Indicate that other cases are impossible
 * }
 * ```
 */
NEX_NORETURN inline void unreachable() {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if NEX_COMPILER_IS_MSVC // MSVC
    // __assume is a hint to the optimizer that the code path is unreachable.
    // It does not generate any code on its own, so we also need to trigger a trap 
    // or abort to ensure the program does not continue executing.
    __assume(false);
    
#else // GCC, Clang
    // __builtin_unreachable() tells the compiler that this code path is unreachable, 
    // which can help with optimization. However, it does not actually cause a crash on its own, 
    // so we also need to trigger a trap or abort to ensure the program does not continue executing
    // if this function is ever called at runtime.
    __builtin_unreachable();
#endif
}

/**
 * @brief Assert handler function pointer type
 * 
 * @details
 * Called when an assertion fails. 
 * The handler receives:
 *  - file: Source file path where assertion failed (cstring)
 *  - line: Line number where assertion failed (int32)
 *  - function: Function name where assertion failed (cstring)
 *  - expression: String representation of the failed expression (cstring)
 *  - message: Optional user-provided message (cstring)
 */
using AssertHandler = void(*)(
    cstring file, 
    int32 line, 
    cstring function, 
    cstring expression, 
    cstring message
);

/**
 * @brief Set the global assert handler
 * 
 * @param handler Function pointer to handle assertion failures. If nullptr, uses default handler.
 * 
 * @note The default handler logs the assertion.
 */
void setAssertHandler(AssertHandler handler);

/**
 * @brief Get the current assert handler
 * 
 * @return Current assert handler function pointer. Returns default handler if no custom handler is set.
 */
AssertHandler getAssertHandler();

NEX_NAMESPACE_END

// ======================================================================================
// Assert/Crash handling macros
// ======================================================================================

/**
 * @def NEX_IMMEDIATE_CRASH()
 * @brief Immediately crash the program
 * 
 * @details
 * This macro triggers an immediate crash using a low-level trap instruction.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * @example
 * ```
 * if (unexpected_condition) {
 *     NEX_IMMEDIATE_CRASH();
 * }
 * ```
 */
#define NEX_IMMEDIATE_CRASH() \
    do { \
        NEX_PREPEND_NAMESPACE(immediateCrash)(); \
    } while (0)

/**
 * @def NEX_FATAL(reason)
 * @brief Immediately crash the program with a specified reason
 * 
 * @details
 * This macro triggers an immediate crash using a low-level trap instruction,
 * providing a reason for the crash.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * @param reason Reason for the crash (const char*)
 * 
 * @example
 * ```
 * if (unexpected_condition) {
 *    NEX_FATAL("Unexpected condition encountered");
 * }
 * ```
 */
#define NEX_FATAL(reason) \
    do { \
        NEX_PREPEND_NAMESPACE(getFatalHandler())(reason, \
            NEX_SOURCE_FILE_PATH, \
            NEX_SOURCE_LINE_NUMBER, \
            NEX_ASSERT_FUNCTION_NAME); \
        NEX_IMMEDIATE_CRASH(); \
    } while (0)

/**
 * @def NEX_UNREACHABLE()
 * @brief Indicate that a code path is unreachable
 * 
 * @details
 * This macro marks a code path as unreachable, allowing the compiler
 * to optimize accordingly. It uses compiler-specific extensions to
 * indicate that the code path should never be executed.
 * 
 * @example
 * ```
 * switch (value) {
 *     case 1:
 *         // Handle case 1
 *         break;
 *     case 2:
 *         // Handle case 2
 *         break;
 *     default:
 *         NEX_UNREACHABLE(); // Indicate that other cases are impossible
 * }
 * ```
 */
#define NEX_UNREACHABLE() \
    do { \
        NEX_PREPEND_NAMESPACE(unreachable)(); \
    } while (0)

/**
 * @def NEX_ASSERT(expr)
 * @brief Assert that an expression evaluates to true
 * 
 * @details
 * If the expression is false, the assert handler is called with file, line, and function information.
 * In debug builds, this will typically terminate the program.
 * In release builds, this will be compiled out and have no effect.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * 
 * @example
 * ```
 * NEX_ASSERT(ptr != nullptr);
 * NEX_ASSERT(index >= 0 && index < count);
 * ```
 */
#if defined(NEX_BUILD_MODE_DEBUG)
    #define NEX_ASSERT(expr) \
        do { \
            if (!(expr)) { \
                NEX_PREPEND_NAMESPACE(getAssertHandler())( \
                    NEX_SOURCE_FILE_PATH, \
                    NEX_SOURCE_LINE_NUMBER, \
                    NEX_ASSERT_FUNCTION_NAME, \
                    #expr, \
                    ""); \
                NEX_IMMEDIATE_CRASH(); \
            } \
        } while (0)
#else
    #define NEX_ASSERT(expr) (void(0))
#endif

/**
 * @def NEX_ASSERT_MSG(expr, msg)
 * @brief Assert that an expression evaluates to true with a custom message
 * 
 * @details
 * Similar to NEX_ASSERT, but includes a user-provided message that will be
 * passed to the assert handler.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * @param msg User message (StringView or convertible to StringView)
 * 
 * @example
 * ```
 * NEX_ASSERT_MSG(value > 0, "Value must be positive");
 * NEX_ASSERT_MSG(buffer != nullptr, "Buffer allocation failed");
 * ```
 */
#if defined(NEX_BUILD_MODE_DEBUG)
    #define NEX_ASSERT_MSG(expr, msg) \
        do { \
            if (!(expr)) { \
                NEX_PREPEND_NAMESPACE(getAssertHandler())( \
                    NEX_SOURCE_FILE_PATH, \
                    NEX_SOURCE_LINE_NUMBER, \
                    NEX_ASSERT_FUNCTION_NAME, \
                    #expr, \
                    msg); \
                NEX_IMMEDIATE_CRASH(); \
            } \
        } while (0)
#else
    #define NEX_ASSERT_MSG(expr, msg) (void(0))
#endif

/**
 * @def NEX_VERIFY(expr)
 * @brief Verify an expression (always checked, even in release builds)
 * 
 * @details
 * Similar to NEX_ASSERT, but is never compiled out, even in release builds.
 * Use this for critical checks that must always be performed.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * 
 * @example
 * ```
 * NEX_VERIFY(InitializeSystem());  // Must always check
 * ```
 */
#define NEX_VERIFY(expr) \
    do { \
        if (!(expr)) { \
            NEX_PREPEND_NAMESPACE(getAssertHandler())( \
                NEX_SOURCE_FILE_PATH, \
                NEX_SOURCE_LINE_NUMBER, \
                NEX_ASSERT_FUNCTION_NAME, \
                #expr, \
                ""); \
            NEX_IMMEDIATE_CRASH(); \
        } \
    } while (0)

/**
 * @def NEX_VERIFY_MSG(expr, msg)
 * @brief Verify an expression with a custom message (always checked)
 * 
 * @details
 * Similar to NEX_VERIFY, but includes a user-provided message.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * @param msg User message (StringView or convertible to StringView)
 * 
 * @example
 * ```
 * NEX_VERIFY_MSG(LoadConfiguration(configPath), "Failed to load configuration");
 * ```
 */
#define NEX_VERIFY_MSG(expr, msg) \
    do { \
        if (!(expr)) { \
            NEX_PREPEND_NAMESPACE(getAssertHandler())( \
                NEX_SOURCE_FILE_PATH, \
                NEX_SOURCE_LINE_NUMBER, \
                NEX_ASSERT_FUNCTION_NAME, \
                #expr, \
                msg); \
            NEX_IMMEDIATE_CRASH(); \
        } \
    } while (0)

/** 
 * @def NEX_NOT_IMPLEMENTED()
 * @brief Indicate that a code path is not yet implemented
 * 
 * @details
 * This macro triggers an assertion failure indicating that the code path
 * is not yet implemented. It is useful as a placeholder during development.
 * 
 * @example
 * ```
 * void myFunction() {
 *     NEX_NOT_IMPLEMENTED();
 * }
 * ```
 */
#define NEX_NOT_IMPLEMENTED() \
    do { \
        NEX_PREPEND_NAMESPACE(getAssertHandler())( \
            NEX_SOURCE_FILE_PATH, \
            NEX_SOURCE_LINE_NUMBER, \
            NEX_ASSERT_FUNCTION_NAME, \
            "Not implemented", \
            "This code path is not yet implemented."); \
        NEX_IMMEDIATE_CRASH(); \
    } while (0)
