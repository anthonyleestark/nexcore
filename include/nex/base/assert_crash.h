/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "common/macros.h"

// Get function name for assertion and fatal handlers
#define NEXSUITE_ASSERT_FUNCTION_NAME \
    NEXSUITE_SOURCE_FUNCTION_NAME

NEXSUITE_NAMESPACE_BEGIN

/**
 * @brief Fatal handler function pointer type
 * 
 * Called when a fatal error is triggered. 
 * The handler receives:
 * - reason: Optional reason for the fatal error (const char*)
 * - file: Source file where the fatal error occurred (const char*)
 * - line: Line number in the source file (int)
 * - function: Function name where the fatal error occurred (const char*)
 * 
 * Example usage:
 * ```
 * void myFatalHandler(const char* reason, const char* file, int line, const char* function) {
 *     // Custom fatal handling logic
 * }
 * ::NEXSUITE_PREPEND_NAMESPACE(setFatalHandler)(myFatalHandler);
 * ```
 */
using FatalHandler = void(*)(
    const char* reason,
    const char* file,
    int line,
    const char* function
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
 * This function triggers an immediate crash using a low-level trap instruction.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * @param reason Optional reason for the crash (currently unused)
 * 
 * Example usage:
 * ```
 * if (unexpected_condition) {
 *     immediateCrash("Unexpected condition encountered");
 * }
 * ```
 */
[[noreturn]] inline void immediateCrash() {
#if NEXSUITE_COMPILER_IS_MSVC && !defined(__clang__)
    // MSVC: __fastfail is a low-level intrinsic that triggers a fast fail, 
    // which is a special kind of crash that is designed to be fatal, unique, and non-allocating. 
    // It does not generate any code on its own, so we also need to trigger a trap or abort 
    // to ensure the program does not continue executing.
    __fastfail(0); // or __fastfail(FAST_FAIL_ILLEGAL_INSTRUCTION);

#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    // GCC / Clang / ICC / AppleClang: trap immediately (illegal instruction)
    __builtin_trap();

#else
    // Fallback: cause a crash by dereferencing a null pointer with an invalid value.
    // This is not guaranteed to be unique or non-allocating, but it's better than nothing 
    // on unsupported platforms.
    *(volatile int*)0 = 0xDEADC0DE;
#endif

    // The function is marked [[noreturn]], so we should never reach this point.
    // If we do, it's an indication that the platform-specific crash mechanism failed 
    // to terminate the program, which is a critical failure. In that case, 
    // we can call std::abort() as a last resort to ensure the program 
    // does not continue executing in an undefined state.
#if defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#endif
}

/**
 * @brief Indicate that a code path is unreachable
 * 
 * This function marks a code path as unreachable, allowing the compiler
 * to optimize accordingly. It uses compiler-specific extensions to
 * indicate that the code path should never be executed.
 * 
 * Example usage:
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
[[noreturn]] inline void unreachable() {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if NEXSUITE_COMPILER_IS_MSVC && !defined(__clang__) // MSVC
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
 * Called when an assertion fails. The handler receives:
 * - file: Source file path where assertion failed
 * - line: Line number where assertion failed
 * - function: Function name where assertion failed (may be empty)
 * - expression: String representation of the failed expression
 * - message: Optional user-provided message (may be empty)
 */
using AssertHandler = void(*)(
    const char* file, 
    int line, 
    const char* function, 
    const char* expression, 
    const char* message
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

NEXSUITE_NAMESPACE_END

/**
 * @def NEXSUITE_IMMEDIATE_CRASH()
 * @brief Immediately crash the program
 * 
 * This macro triggers an immediate crash using a low-level trap instruction.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * Example usage:
 * ```
 * if (unexpected_condition) {
 *     NEXSUITE_IMMEDIATE_CRASH();
 * }
 * ```
 */
#define NEXSUITE_IMMEDIATE_CRASH() \
    do { \
        NEXSUITE_PREPEND_NAMESPACE(immediateCrash)(); \
    } while (0)

/**
 * @def NEXSUITE_FATAL(reason)
 * @brief Immediately crash the program with a specified reason
 * 
 * This macro triggers an immediate crash using a low-level trap instruction,
 * providing a reason for the crash.
 * It is designed to be fatal, unique, and non-allocating, ensuring that
 * crash reports remain accurate and debuggable.
 * 
 * @param reason Reason for the crash (const char*)
 * 
 * Example usage:
 * ```
 * if (unexpected_condition) {
 *    NEXSUITE_FATAL("Unexpected condition encountered");
 * }
 * ```
 */
#define NEXSUITE_FATAL(reason) \
    do { \
        NEXSUITE_PREPEND_NAMESPACE(getFatalHandler())(reason, \
            NEXSUITE_SOURCE_FILE_PATH, \
            NEXSUITE_SOURCE_LINE_NUMBER, \
            NEXSUITE_ASSERT_FUNCTION_NAME); \
        NEXSUITE_IMMEDIATE_CRASH(); \
    } while (0)

/**
 * @def NEXSUITE_UNREACHABLE()
 * @brief Indicate that a code path is unreachable
 * 
 * This macro marks a code path as unreachable, allowing the compiler
 * to optimize accordingly. It uses compiler-specific extensions to
 * indicate that the code path should never be executed.
 * 
 * Example usage:
 * ```
 * switch (value) {
 *     case 1:
 *         // Handle case 1
 *         break;
 *     case 2:
 *         // Handle case 2
 *         break;
 *     default:
 *         NEXSUITE_UNREACHABLE(); // Indicate that other cases are impossible
 * }
 * ```
 */
#define NEXSUITE_UNREACHABLE() \
    do { \
        NEXSUITE_PREPEND_NAMESPACE(unreachable)(); \
    } while (0)

/**
 * @def NEXSUITE_ASSERT(expr)
 * @brief Assert that an expression evaluates to true
 * 
 * If the expression is false, the assert handler is called with file, line, and function information.
 * In debug builds, this will typically terminate the program.
 * In release builds, this will be compiled out and have no effect.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * 
 * @example
 * ```
 * NEXSUITE_ASSERT(ptr != nullptr);
 * NEXSUITE_ASSERT(index >= 0 && index < count);
 * ```
 */
#if defined(NEXSUITE_BUILD_DEBUG)
    #define NEXSUITE_ASSERT(expr) \
        do { \
            if (!(expr)) { \
                NEXSUITE_PREPEND_NAMESPACE(getAssertHandler())( \
                    NEXSUITE_SOURCE_FILE_PATH, \
                    NEXSUITE_SOURCE_LINE_NUMBER, \
                    NEXSUITE_ASSERT_FUNCTION_NAME, \
                    #expr, \
                    ""); \
                NEXSUITE_IMMEDIATE_CRASH(); \
            } \
        } while (0)
#else
    #define NEXSUITE_ASSERT(expr) (void(0))
#endif

/**
 * @def NEXSUITE_ASSERT_MSG(expr, msg)
 * @brief Assert that an expression evaluates to true with a custom message
 * 
 * Similar to NEXSUITE_ASSERT, but includes a user-provided message that will be
 * passed to the assert handler.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * @param msg User message (StringView or convertible to StringView)
 * 
 * @example
 * ```
 * NEXSUITE_ASSERT_MSG(value > 0, "Value must be positive");
 * NEXSUITE_ASSERT_MSG(buffer != nullptr, "Buffer allocation failed");
 * ```
 */
#if defined(NEXSUITE_BUILD_DEBUG)
    #define NEXSUITE_ASSERT_MSG(expr, msg) \
        do { \
            if (!(expr)) { \
                NEXSUITE_PREPEND_NAMESPACE(getAssertHandler())( \
                    NEXSUITE_SOURCE_FILE_PATH, \
                    NEXSUITE_SOURCE_LINE_NUMBER, \
                    NEXSUITE_ASSERT_FUNCTION_NAME, \
                    #expr, \
                    msg); \
                NEXSUITE_IMMEDIATE_CRASH(); \
            } \
        } while (0)
#else
    #define NEXSUITE_ASSERT_MSG(expr, msg) (void(0))
#endif

/**
 * @def NEXSUITE_VERIFY(expr)
 * @brief Verify an expression (always checked, even in release builds)
 * 
 * Similar to NEXSUITE_ASSERT, but is never compiled out, even in release builds.
 * Use this for critical checks that must always be performed.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * 
 * Example usage:
 * ```
 * NEXSUITE_VERIFY(InitializeSystem());  // Must always check
 * ```
 */
#define NEXSUITE_VERIFY(expr) \
    do { \
        if (!(expr)) { \
            NEXSUITE_PREPEND_NAMESPACE(getAssertHandler())( \
                NEXSUITE_SOURCE_FILE_PATH, \
                NEXSUITE_SOURCE_LINE_NUMBER, \
                NEXSUITE_ASSERT_FUNCTION_NAME, \
                #expr, \
                ""); \
            NEXSUITE_IMMEDIATE_CRASH(); \
        } \
    } while (0)

/**
 * @def NEXSUITE_VERIFY_MSG(expr, msg)
 * @brief Verify an expression with a custom message (always checked)
 * 
 * Similar to NEXSUITE_VERIFY, but includes a user-provided message.
 * 
 * @param expr Expression to check (must be convertible to bool)
 * @param msg User message (StringView or convertible to StringView)
 * 
 * Example usage:
 * ```
 * NEXSUITE_VERIFY_MSG(LoadConfiguration(configPath), "Failed to load configuration");
 * ```
 */
#define NEXSUITE_VERIFY_MSG(expr, msg) \
    do { \
        if (!(expr)) { \
            NEXSUITE_PREPEND_NAMESPACE(getAssertHandler())( \
                NEXSUITE_SOURCE_FILE_PATH, \
                NEXSUITE_SOURCE_LINE_NUMBER, \
                NEXSUITE_ASSERT_FUNCTION_NAME, \
                #expr, \
                msg); \
            NEXSUITE_IMMEDIATE_CRASH(); \
        } \
    } while (0)

/** 
 * @def NEXSUITE_NOT_IMPLEMENTED()
 * @brief Indicate that a code path is not yet implemented
 * 
 * This macro triggers an assertion failure indicating that the code path
 * is not yet implemented. It is useful as a placeholder during development.
 * 
 * Example usage:
 * ```
 * void myFunction() {
 *     NEXSUITE_NOT_IMPLEMENTED();
 * }
 * ```
 */
#define NEXSUITE_NOT_IMPLEMENTED() \
    do { \
        NEXSUITE_PREPEND_NAMESPACE(getAssertHandler())( \
            NEXSUITE_SOURCE_FILE_PATH, \
            NEXSUITE_SOURCE_LINE_NUMBER, \
            NEXSUITE_ASSERT_FUNCTION_NAME, \
            "Not implemented", \
            "This code path is not yet implemented."); \
        NEXSUITE_IMMEDIATE_CRASH(); \
    } while (0)
