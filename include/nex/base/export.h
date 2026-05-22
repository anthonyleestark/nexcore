/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   export.h
 * @brief  Define macros for marking symbols for export or import in shared libraries and 
 *         for specifying calling conventions for callback functions.
 * 
 * @details
 * This header defines macros for marking symbols for export or import in shared libraries and 
 * for specifying calling conventions for callback functions. 
 * The NEX_API macro is defined based on the compiler and build configuration, while the NEX_CALLBACK 
 * macro is defined based on the detected platform.
 */

#include "nex/base/platform.h"
#include "nex/base/compiler.h"
#include "nex/base/build.h"

// ======================================================================================
// Shared Library Export/Import Macros
// ======================================================================================

/**
 * @details
 * The NEX_API macro is defined based on the compiler and build configuration.
 * - On MSVC, it expands to `__declspec(dllexport)` when building the shared library, and `__declspec(dllimport)` 
 *   when using the shared library.
 * - On GCC, it expands to `__attribute__((visibility("default")))` to make symbols visible for export.
 * - If the compiler does not support these attributes, it expands to nothing, allowing the code to compile 
 *   without any errors.
 */

#if NEX_COMPILER_MSVC_COMPATIBLE
    // On MSVC, it expands to `__declspec(dllexport)` when building the shared library, 
    // and `__declspec(dllimport)` when using the shared library.
    #if NEX_BUILDING_SHARED
        #define NEX_API __declspec(dllexport)
    #elif NEX_USING_SHARED
        #define NEX_API __declspec(dllimport)
    #endif
#elif NEX_COMPILER_GCC_COMPATIBLE
    // On GCC, it expands to `__attribute__((visibility("default")))` 
    // to make symbols visible for export, only when building the shared library.
    #if NEX_BUILDING_SHARED
        #define NEX_API __attribute__((visibility("default")))
    #endif
#endif // NEX_API

#if !defined(NEX_API)
    // If the compiler does not support these attributes, it expands to nothing, 
    // allowing the code to compile without any errors.
    #define NEX_API
#endif  // !defined(NEX_API)

// ======================================================================================
// Calling Convention Macros
// ======================================================================================

/**
 * @def NEX_CALLBACK
 * @brief Define a calling convention for callback functions
 * 
 * @details
 * This macro can be used to specify a calling convention for callback functions, which is important for ensuring 
 * compatibility with different platforms and APIs. For example, on Windows, the `__stdcall` calling convention is 
 * commonly used for callbacks in the Windows API. On other platforms, the default calling convention is typically used.
 * The NEX_CALLBACK macro expands to the appropriate calling convention based on the detected platform, allowing you 
 * to write portable code that can be used across different operating systems and APIs without modification.
 * 
 * Example usage:
 * @code
 *   void NEX_CALLBACK MyCallbackFunction(int param) { ... }
 * @endcode
 */

#if NEX_PLATFORM_IS_WINDOWS
    // On Windows, use __stdcall calling convention for compatibility with Windows API 
    // and consistent calling convention across the framework.
    #define NEX_CALLBACK __stdcall
#else
    // On non-Windows platforms, the default calling convention is typically used, 
    // so no special attributes are needed.
    #define NEX_CALLBACK
#endif  // NEX_PLATFORM_IS_WINDOWS

/**
 * @def NEX_CDECL
 * @brief Specify the C declaration calling convention
 * 
 * @details
 * Expands to `__cdecl` on Windows, empty on other platforms.
 * Useful for writing cross-platform function pointers that use the C calling convention.
 */
#if !defined(NEX_CDECL)
    #if NEX_PLATFORM_IS_WINDOWS
        #define NEX_CDECL __cdecl
    #else  // On non-Windows platforms
        #define NEX_CDECL
    #endif  // NEX_PLATFORM_IS_WINDOWS
#endif  // !defined(NEX_CDECL)

// ======================================================================================
// Symbol Modifiers
// ======================================================================================

/**
 * @def NEX_INTERNAL
 * @brief Mark a symbol as internal (not exported) for shared libraries
 * 
 * @details
 * This macro can be used to mark symbols as internal, meaning they will not be exported from the shared library. 
 * This can help to reduce the exported symbol count and improve encapsulation by hiding implementation details. 
 * The NEX_INTERNAL macro expands to the appropriate compiler-specific attribute based on the detected compiler. 
 * On Windows (MSVC), there is no direct equivalent, so it expands to nothing. If the compiler does not support 
 * a visibility attribute, it also expands to nothing.
 * On macOS/Linux (GCC/Clang), it expands to `__attribute__((visibility("hidden")))`. 
 */

#if NEX_PLATFORM_IS_WINDOWS
    // On Windows, there is no direct equivalent to marking symbols as internal.
    #define NEX_INTERNAL
#else
    // On macOS/Linux (GCC/Clang), we can use the visibility attribute to hide symbols from being exported.
    #define NEX_INTERNAL __attribute__((visibility("hidden")))
#endif  // NEX_PLATFORM_IS_WINDOWS
