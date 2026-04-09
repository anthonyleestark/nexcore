/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      attributes.h
 * @brief     Defines macros for common attributes and annotations used in the codebase, such as export/import, 
 *            deprecation, nodiscard, and noreturn.
 * 
 * @details
 * This section defines macros for marking functions and classes with attributes such as export/import (for shared libraries), 
 * deprecation (with or without a message), nodiscard (to indicate that the return value should not be ignored), and noreturn 
 * (to indicate that a function does not return).
 * The macros are defined based on the compiler being used, and they can be used throughout the codebase to provide 
 * consistent annotations and attributes for functions and classes.
 * 
 * @note When using the NEX_DEPRECATED macro, it is recommended to provide a message that explains why 
 *       the function or class is deprecated and what should be used instead.
 */

#include "nex/base/compiler.h"

////// Exported macros for shared library support =========================================================

#if NEX_COMPILER_IS_MSVC

    #if defined(NEX_BUILDING_SHARED)
        #define NEX_EXPORT __declspec(dllexport)
    #elif defined(NEX_USING_SHARED)
        #define NEX_EXPORT __declspec(dllimport)
    #else
        #define NEX_EXPORT
    #endif

#elif NEX_COMPILER_IS_GCC
    #define NEX_EXPORT __attribute__((visibility("default")))
#endif  // NEX_COMPILER_IS_MSVC

// Undefine min/max macros if defined 
// (to avoid conflicts with std::min/std::max)
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

////// Attributes and annotations =========================================================

// Check for built-in __float128 support (only on GCC/Clang) and define a macro accordingly
#if defined(__SIZEOF_FLOAT128__) && !NEX_COMPILER_IS_MSVC
    #define NEX_HAS_BUILTIN_FLOAT128 1
#else
    #define NEX_HAS_BUILTIN_FLOAT128 0
#endif

// [[nodiscard]] attributes on STL functions
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

// Mark a function as not returning (e.g., it may throw an exception or call std::terminate)
#if NEX_COMPILER_IS_MSVC
    #define NEX_NORETURN __declspec(noreturn)
#elif NEX_COMPILER_IS_GCC
    #define NEX_NORETURN __attribute__((noreturn))
#else
    #define NEX_NORETURN
#endif

// Mark a function as deprecated with an optional message
#if NEX_COMPILER_IS_MSVC
    #define NEX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif NEX_COMPILER_IS_GCC
    #define NEX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
    #define NEX_DEPRECATED(msg)
#endif

// Mark a function as deprecated without a message
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
