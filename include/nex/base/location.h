/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    location.h
 * @brief   Define macros and utilities for capturing source code location information (file, line, function, etc.) 
 *          for debugging, logging, analysis, and error reporting purposes.
 * 
 * @details
 * This header defines macros and utilities for capturing source code location information, such as file name, line 
 * number, and function name. This information can be used for debugging, logging, analysis, and error reporting purposes
 * to provide more context about where in the source code an event occurred. The macros defined in this header are designed
 * to be easy to use and provide consistent source location information across different compilers and platforms,
 * making it easier to maintain and debug code in a cross-platform environment.
 */

#include "nex/base/compiler.h"
#include "nex/base/build.h"

// ================================================================================================
// Compiler built-in attribute detection
// ================================================================================================

// Check for support of compiler built-ins for capturing source location information 
// (e.g., __builtin_FILE(), __builtin_LINE(), __builtin_FUNCTION()) and define a macro accordingly.
#if !defined(NEX_HAS_BUILTIN_SOURCE_LOCATION)
    #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_MSVC_COMPATIBLE
        #define NEX_HAS_BUILTIN_SOURCE_LOCATION 1
    #else  // Compiler does not support __builtin_FILE/LINE/FUNCTION()
        #define NEX_HAS_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_MSVC_COMPATIBLE
#endif  // !defined(NEX_HAS_BUILTIN_SOURCE_LOCATION)

// User-configurable macro to enable/disable the use of compiler built-ins for capturing source location information.
#if !defined(NEX_USE_BUILTIN_SOURCE_LOCATION)
    #if NEX_HAS_BUILTIN_SOURCE_LOCATION
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 1
    #else  // Compiler does not support built-in source location
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_HAS_BUILTIN_SOURCE_LOCATION
#else
    // If the user has already defined NEX_USE_BUILTIN_SOURCE_LOCATION, we use their definition, 
    // but we also check if it's set to 1 while the compiler does not support it, in which case 
    // we disable it to avoid compilation errors.
    #if NEX_USE_BUILTIN_SOURCE_LOCATION && !NEX_HAS_BUILTIN_SOURCE_LOCATION
        #undef NEX_USE_BUILTIN_SOURCE_LOCATION
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_USE_BUILTIN_SOURCE_LOCATION && !NEX_HAS_BUILTIN_SOURCE_LOCATION
#endif  // !defined(NEX_USE_BUILTIN_SOURCE_LOCATION)

// User-configurable macro to enable/disable the use of compiler built-ins for capturing detailed function signatures 
// (e.g., __PRETTY_FUNCTION__, __FUNCSIG__), which can provide more detailed information about the function, such as 
// parameter types and template arguments, but may be less readable than just the function name, so we allow the user 
// to choose whether to use the detailed function signature or just the function name for source location information.
#if !defined(NEX_USE_DETAILED_FUNCTION_SIGNATURE)
    #define NEX_USE_DETAILED_FUNCTION_SIGNATURE 1
#else
    // If the user has already defined NEX_USE_DETAILED_FUNCTION_SIGNATURE, we use their definition, 
    // but we redefine it into a boolean value (1 or 0) to ensure consistent usage in the codebase.
    #undef NEX_USE_DETAILED_FUNCTION_SIGNATURE
    #define NEX_USE_DETAILED_FUNCTION_SIGNATURE 1
#endif  // !defined(NEX_USE_DETAILED_FUNCTION_SIGNATURE)

// ================================================================================================
// Helper functions for stripping paths and function signatures
// ================================================================================================

// Strip the path from a file path, returning only the file name, which can be used for various purposes 
// such as improving the readability of error messages and logs by showing only the file name instead of 
// the full file path.
constexpr const char* stripFilePath(const char* filePath) {
    if (!filePath) return "";
    const char* file = filePath;
    for (const char* p = filePath; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            file = p + 1;
        }
    }
    return file;
}

// Get the function name from a function signature, which can be used for various purposes such as improving the 
// readability of error messages and logs by showing only the function name instead of the full function signature.
constexpr const char* stripFunctionSignature(const char* funcSignature) {
    if (!funcSignature) return "";
    const char* func = funcSignature;
    for (const char* p = funcSignature; *p; ++p) {
        if (*p == '(') {
            func = p;
            break;
        }
    }
    for (const char* p = func; p > funcSignature; --p) {
        if (*p == ' ' || *p == ':') {
            func = p + 1;
            break;
        }
    }
    return func;
}

// ================================================================================================
// Source location metadata macros 
// ================================================================================================

// Get the full file path for source location
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #define NEX_SOURCE_FILE_PATH __builtin_FILE()
#else  // Compiler does not support __builtin_FILE()
    #define NEX_SOURCE_FILE_PATH __FILE__
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

// Get the line number for source location
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #define NEX_SOURCE_LINE_NUMBER static_cast<int>(__builtin_LINE())
#else  // Compiler does not support __builtin_LINE()
    #define NEX_SOURCE_LINE_NUMBER __LINE__
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

// Get the function name for source location
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #if NEX_USE_DETAILED_FUNCTION_SIGNATURE
        #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCTION()
        #elif NEX_COMPILER_IS_MSVC
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCSIG()
        #elif NEX_COMPILER_MSVC_COMPATIBLE
            // Clang on MSVC supports __builtin_FUNCTION() and provides more detailed function signatures 
            // than __builtin_FUNCSIG(), so we use __builtin_FUNCTION() for better readability and consistency 
            // with other platforms.
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCTION()
        #endif
    #else  // User prefers short function names, so we strip the function signature to get only the function name
        // We can use __builtin_FUNCTION() for all compilers that support it
        #define NEX_SOURCE_FUNCTION_NAME stripFunctionSignature(__builtin_FUNCTION())
    #endif  // !NEX_USE_DETAILED_FUNCTION_SIGNATURE
#else  // Compiler does not support __builtin_FUNCTION()
    #if NEX_USE_DETAILED_FUNCTION_SIGNATURE
        #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE
            #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
        #elif NEX_COMPILER_IS_MSVC
            #define NEX_SOURCE_FUNCTION_NAME __FUNCSIG__
        #elif NEX_COMPILER_MSVC_COMPATIBLE
            // Clang on MSVC supports __PRETTY_FUNCTION__ and provides more detailed function signatures than __FUNCSIG__, 
            // so we use __PRETTY_FUNCTION__ for better readability and consistency with other platforms.
            #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
        #else
            // See: https://en.cppreference.com/w/c/language/function_definition#func
            #define NEX_SOURCE_FUNCTION_NAME __func__
        #endif
    #else  // User prefers short function names
        // See also: https://en.cppreference.com/w/c/language/function_definition#func
        #define NEX_SOURCE_FUNCTION_NAME __func__
    #endif
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

// ================================================================================================
// Use C++20 std::source_location
// ================================================================================================

// User-configurable macro to enable/disable the use of std::source_location for capturing source location information.
#if defined(NEX_USE_STD_SOURCE_LOCATION)
    #if NEX_COMPILER_IS_MSVC
        // MSVC's implementation of std::source_location is incomplete and does not support all the features we need, 
        // so we disable it for now and use our own implementation instead.
        #undef NEX_USE_STD_SOURCE_LOCATION
        #define NEX_USE_STD_SOURCE_LOCATION 0
    #else
        // On GCC and Clang, std::source_location is fully supported and provides a more standardized way 
        // to capture source location information, so we enable it if the user has requested it and it's available 
        // because we are compiling with C++20 or later.
        #undef NEX_USE_STD_SOURCE_LOCATION
        #define NEX_USE_STD_SOURCE_LOCATION 1
    #endif
#else
    // If the user has not defined NEX_USE_STD_SOURCE_LOCATION, we default to 0 (disabled) to avoid compilation 
    // errors on compilers and use our own implementation instead.
    #define NEX_USE_STD_SOURCE_LOCATION 0
#endif
