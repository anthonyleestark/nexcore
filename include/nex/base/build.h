/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   build.h
 * @brief  Defines macros for build configuration, such as build mode (debug/release), source location information,
 *         build linkage detection (shared/static), and build environment detection (target architecture, CI/CD).
 * 
 * @details
 * This header centralizes macros related to build configuration and environment detection.
 * It includes macros for detecting build mode (debug or release), capturing source location information 
 * (file, line, function), detecting whether we are building or using a shared library, and detecting 
 * the target architecture (x86/x64), as well as integration with CI/CD environments.
 */

#include "nex/base/compiler.h"

// ======================================================================================
// Building mode detection
// ======================================================================================

// Check if we are in a debug build mode and define a macro accordingly
// The logic for detecting a debug build mode is based on common macros defined by compilers and build systems, 
// such as NDEBUG, _DEBUG, and optimization flags.
#if !defined(NEX_BUILD_MODE_DEBUG)
    #if !defined(NDEBUG) || defined(_DEBUG)
        // Check if we are NOT optimizing (only works in GCC/Clang)
        #if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
            #if !defined(__OPTIMIZE__)
                #define NEX_BUILD_MODE_DEBUG
                #undef NEX_BUILD_MODE_RELEASE
            #endif
        #else
            // Fallback for other compilers/MSVC logic
            #define NEX_BUILD_MODE_DEBUG
            #undef NEX_BUILD_MODE_RELEASE
        #endif
    #endif
#endif

// Check if we are in a release build mode and define a macro accordingly
// The logic for detecting a release build mode is based on common macros defined by compilers and build systems,
// such as NDEBUG, _DEBUG, and optimization flags.
#if !defined(NEX_BUILD_MODE_RELEASE) && !defined(NEX_BUILD_MODE_DEBUG)
    #if defined(NDEBUG) && !defined(_DEBUG)
        #if NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_IS_CLANG
            #if defined(__OPTIMIZE__)
                #define NEX_BUILD_MODE_RELEASE
                #undef NEX_BUILD_MODE_DEBUG
            #endif
        #else
            // Fallback for other compilers/MSVC logic
            #define NEX_BUILD_MODE_RELEASE
            #undef NEX_BUILD_MODE_DEBUG
        #endif
    #endif
#endif

// Define a macro for checking which build configuration we are in (debug or release)
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
// Macros defining for source location information (typically used in debug builds)
// ======================================================================================

// Get the full file path for source location
#define NEX_SOURCE_FILE_PATH __FILE__

// Get the line number for source location
#define NEX_SOURCE_LINE_NUMBER __LINE__

// Get the function name for source location
#if NEX_COMPILER_GCC_COMPATIBLE
    #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
#elif NEX_COMPILER_IS_MSVC
    #define NEX_SOURCE_FUNCTION_NAME __FUNCSIG__
#elif NEX_COMPILER_MSVC_COMPATIBLE
    // Clang on MSVC supports __PRETTY_FUNCTION__ and provides more detailed function signatures than __FUNCSIG__, 
    // so we use __PRETTY_FUNCTION__ for better readability and consistency with other platforms. 
    #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
#else
    // See https://en.cppreference.com/w/c/language/function_definition#func
    #define NEX_SOURCE_FUNCTION_NAME __func__
#endif

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
// Build environment detection (e.g., target environment (x86/x64), CI/CD, etc.)
// ======================================================================================

// Detect if we are building for a 64-bit or 32-bit environment based on common macros 
// defined by compilers and build systems, using a combination of predefined macros that 
// indicate the target environment, such as __LP64__, _WIN64, and size of pointer.
#if defined(__LP64__) || defined(_WIN64) || \
    (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8)
    // We are building for a 64-bit environment (x64)
    #define NEX_BUILD_ENV_64_BIT
    #undef NEX_BUILD_ENV_IS_64_BIT
    #define NEX_BUILD_ENV_IS_64_BIT 1
    #undef NEX_BUILD_ENV_32_BIT
    #define NEX_BUILD_ENV_IS_32_BIT 0
#else
    // We are building for a 32-bit environment (x86)
    #define NEX_BUILD_ENV_32_BIT
    #undef NEX_BUILD_ENV_IS_64_BIT
    #define NEX_BUILD_ENV_IS_64_BIT 0
    #undef NEX_BUILD_ENV_IS_32_BIT
    #define NEX_BUILD_ENV_IS_32_BIT 1
#endif

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
