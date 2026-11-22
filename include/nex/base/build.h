/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  build.h
 * @brief Provides build configuration and linkage macros.
 *
 * @details
 * This header centralizes compile-time configuration related to how NexCore and its consuming code are built.
 *
 * It provides macros for identifying the active build mode, such as debug or release, and for determining
 * whether NexCore is being built as a shared library or consumed as a shared library.
 *
 * The resulting macros allow source files to adapt their behavior according to the active build configuration.
 *
 * @note
 * Build configuration should preferably be supplied explicitly by the build system.
 * Compiler-provided macros may be used as fallbacks where appropriate.
 */

#include "nex/base/compiler.h"

// ======================================================================================
// Building mode detection
// ======================================================================================

// Check if we are in a debug build mode or release build mode and define a macro accordingly
// The logic for auto-detecting the build mode is based on common macros defined by compilers and build systems,
// such as NDEBUG, _DEBUG, and optimization flags.

// First we check if user has explicitly defined the build mode macros
// If not, we will try to detect the build mode based on common macros defined by compilers and build systems.
#if !defined(NEX_BUILD_MODE_DEBUG) && !defined(NEX_BUILD_MODE_RELEASE)
    // Highest priority: check for NDEBUG, which is commonly defined in release builds
    #if defined(NDEBUG)
        #define NEX_BUILD_MODE_RELEASE
    // Next priority: check for _DEBUG/DEBUG, 
    // which is commonly defined in debug builds on Windows or in some build systems
    #elif defined(_DEBUG) || defined(DEBUG)
        #define NEX_BUILD_MODE_DEBUG
    // Next priority: check for optimization flags, which are commonly defined in release builds on GCC and Clang
    #elif (NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG_BASED) && defined(__OPTIMIZE__)
        #define NEX_BUILD_MODE_RELEASE
    // If we cannot detect the build mode, we can default to release mode,
    // but this should not happen in a properly configured build environment.
    #else
        #define NEX_BUILD_MODE_RELEASE 
    #endif
#endif

// We check for conflicting definitions of build mode macros and resolve them by prioritizing one over the other,
// or by generating a compile-time error to alert the developer to fix the build configuration.
#if defined(NEX_BUILD_MODE_DEBUG) && defined(NEX_BUILD_MODE_RELEASE)
    // We prioritize release mode in this case
    #undef NEX_BUILD_MODE_DEBUG
#endif

// Define boolean macros for checking which build configuration we are in (debug or release)
#if defined(NEX_BUILD_MODE_DEBUG)
    #define NEX_BUILD_MODE_IS_DEBUG 1
    #define NEX_BUILD_MODE_IS_RELEASE 0
#elif defined(NEX_BUILD_MODE_RELEASE)
    #define NEX_BUILD_MODE_IS_DEBUG 0
    #define NEX_BUILD_MODE_IS_RELEASE 1
#else 
    // Fallback: if we cannot detect the build configuration, define both as 0 to avoid compilation errors,
    // but this should not happen in a properly configured build environment.
    #define NEX_BUILD_MODE_IS_DEBUG 0
    #define NEX_BUILD_MODE_IS_RELEASE 0
#endif

// ======================================================================================
// Build linkage detection (building/using shared library or static library)
// ======================================================================================

#if defined(NEX_BUILDING_SHARED)
    // We are building a shared library (DLL on Windows, .so on Linux, .dylib on macOS)
    #undef NEX_BUILDING_SHARED
    #define NEX_BUILDING_SHARED 1
    #undef NEX_USING_SHARED
    #define NEX_USING_SHARED 0
#elif defined(NEX_USING_SHARED)
    // We are using a shared library (DLL on Windows, .so on Linux, .dylib on macOS)
    #undef NEX_USING_SHARED
    #define NEX_USING_SHARED 1
    #undef NEX_BUILDING_SHARED
    #define NEX_BUILDING_SHARED 0
#else
    // We are building/using a static library (default if neither NEX_BUILDING_SHARED nor NEX_USING_SHARED is defined)
    #define NEX_BUILDING_STATIC_LIBRARY 1
    #undef NEX_BUILDING_SHARED
    #define NEX_BUILDING_SHARED 0
    #undef NEX_USING_SHARED
    #define NEX_USING_SHARED 0
#endif

// ======================================================================================
// Build environment detection (e.g., CI/CD, etc.)
// ======================================================================================

// Detect if we are building in a CI/CD environment based on common macros defined by CI/CD systems,
// such as CI, BUILD_NUMBER, and GITHUB_ACTIONS.
#if defined(CI) || defined(BUILD_NUMBER) || defined(GITHUB_ACTIONS)
    // We are building in a CI/CD environment
    #define NEX_BUILD_ENV_CI_CD
    #undef NEX_BUILD_ENV_IS_CI_CD
    #define NEX_BUILD_ENV_IS_CI_CD 1
#else
    // We are building in a local development environment
    #undef NEX_BUILD_ENV_IS_CI_CD
    #define NEX_BUILD_ENV_IS_CI_CD 0
#endif
