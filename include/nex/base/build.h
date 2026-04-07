/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      build_config.h
 * @defgroup  build_config_detection Build Configuration Detection
 * @brief     Detects the build configuration (e.g., debug, release) and defines macros for conditional compilation 
 *            based on the build configuration; also defines helper macros for source location, which can be used for 
 *            logging, error reporting, and debugging purposes.
 */

#include "nex/base/compiler.h"

// Check if we are in a debug build and define a macro accordingly
// The logic for detecting a debug build is based on common macros defined by compilers and build systems, 
// such as NDEBUG, _DEBUG, and optimization flags.
#if !defined(NEX_BUILD_DEBUG)
    #if !defined(NDEBUG) || defined(_DEBUG)
        // Check if we are NOT optimizing (only works in GCC/Clang)
        #if defined(__GNUC__) || defined(__clang__)
            #if !defined(__OPTIMIZE__)
                #define NEX_BUILD_DEBUG
            #endif
        #else
            // Fallback for other compilers/MSVC logic
            #define NEX_BUILD_DEBUG
        #endif
    #endif
#endif

// Check if we are in a release build and define a macro accordingly
// The logic for detecting a release build is based on common macros defined by compilers and build systems,
// such as NDEBUG, _DEBUG, and optimization flags.
#if !defined(NEX_BUILD_RELEASE) && !defined(NEX_BUILD_DEBUG)
    #if defined(NDEBUG) && !defined(_DEBUG)
        #if defined(__GNUC__) || defined(__clang__)
            #if defined(__OPTIMIZE__)
                #define NEX_BUILD_RELEASE
            #endif
        #else
            // Fallback for other compilers/MSVC logic
            #define NEX_BUILD_RELEASE
        #endif
    #endif
#endif

// Define a macro for checking which build configuration we are in (debug or release)
#if defined(NEX_BUILD_DEBUG)
    #define NEX_BUILD_MODE_IS_DEBUG 1
    #define NEX_BUILD_MODE_IS_RELEASE 0
#elif defined(NEX_BUILD_RELEASE)
    #define NEX_BUILD_MODE_IS_DEBUG 0
    #define NEX_BUILD_MODE_IS_RELEASE 1
#else 
    // Fallback: if we cannot detect the build configuration, define both as 0 to avoid compilation errors, 
    // but this should not happen in a properly configured build environment.
    #define NEX_BUILD_MODE_IS_DEBUG 0
    #define NEX_BUILD_MODE_IS_RELEASE 0
#endif

// Get the full file path for source location
#define NEX_SOURCE_FILE_PATH __FILE__

// Get the line number for source location
#define NEX_SOURCE_LINE_NUMBER __LINE__

// Get the function name for source location
#if NEX_COMPILER_IS_GCC
    #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
#elif NEX_COMPILER_IS_MSVC && !defined(__clang__)
    #define NEX_SOURCE_FUNCTION_NAME __FUNCSIG__
#elif NEX_COMPILER_IS_MSVC && defined(__clang__)
    // Clang on MSVC supports __PRETTY_FUNCTION__ and provides more detailed function signatures than __FUNCSIG__, 
    // so we use __PRETTY_FUNCTION__ for better readability and consistency with other platforms. 
    #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
#else
    // See https://en.cppreference.com/w/c/language/function_definition#func
    #define NEX_SOURCE_FUNCTION_NAME __func__
#endif

// User-configurable macro to enable/disable the use of std::source_location for capturing source location information.
#if defined(NEX_USE_STD_SOURCE_LOCATION)
    #if NEX_COMPILER_IS_MSVC && !defined(__clang__)
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
