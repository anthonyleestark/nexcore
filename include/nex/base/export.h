/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  export.h
 * @brief Defines shared-library symbol export and import macros.
 *
 * @details
 * This header provides a consistent interface for marking symbols that are exported from
 * or imported into shared libraries.
 *
 * It adapts symbol export and import declarations according to the active compiler, platform,
 * and build configuration.
 *
 * These macros allow public APIs to be declared consistently regardless of whether NexCore
 * is built as a shared library or consumed by another binary.
 *
 * @note
 * Low-level symbol visibility annotations are provided separately by visibility.h.
 *
 * @see visibility.h
 * @see build.h
 */

#include "nex/base/platform.h"
#include "nex/base/compiler.h"
#include "nex/base/compiler_features.h"
#include "nex/base/build.h"
#include "nex/base/visibility.h"

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
        #define NEX_API NEX_MSVC_DECLSPEC(dllexport)
    #elif NEX_USING_SHARED
        #define NEX_API NEX_MSVC_DECLSPEC(dllimport)
    #endif
#elif NEX_COMPILER_GCC_COMPATIBLE
    // On GCC, it expands to `__attribute__((visibility("default")))`
    // to make symbols visible for export, only when building the shared library.
    #if NEX_BUILDING_SHARED
        #define NEX_API NEX_VISIBILITY(default)
    #endif
#endif  // ^^NEX_API

#if !defined(NEX_API)
    // If the compiler does not support these attributes, it expands to nothing,
    // allowing the code to compile without any errors.
    #define NEX_API
#endif  // !defined(NEX_API)

/**
 * @def NEX_EXPORTED_FROM_ABI
 * @brief Marks a symbol as exported from the shared library.
 * @note
 * This macro should be used for symbols whose definitions are hidden within the shared library
 * but need to be accessible externally, while NEX_API is used for general symbol export/import.
 */
#define NEX_EXPORTED_FROM_ABI NEX_API

/**
 * @def NEX_UNEXPORTED
 * @brief Marks a symbol as not exported from the shared library.
 */
#if NEX_COMPILER_IS_MSVC
    // On MSVC, there is no direct equivalent to hiding symbols, so we define it as empty.
    #define NEX_UNEXPORTED
#else
    // On non-MSVC compilers, we can use the visibility attribute to hide symbols.
    #define NEX_UNEXPORTED NEX_HIDDEN
#endif  // ^^NEX_UNEXPORTED

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
#endif  // ^^NEX_CALLBACK

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
    #endif
#endif  // ^^NEX_CDECL
